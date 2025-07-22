#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ph_sensor.h>
#include <../mocks/mock_ad_converter.h>
#include <../mocks/mock_calib_storage.h>
#include <../mocks/mock_stability_checker.h>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::StrictMock;

// Keine Namenskonflikte durch anonymen namespace
namespace
{
	static constexpr uint8_t TEST_ADC_CHANNEL = 0;
	static constexpr float TEST_TEMPERATURE = 25.0f;
	static constexpr uint16_t TEST_VOLTAGE = 1500;
	static constexpr float TEST_PH_VALUE = 7.0f;
	static constexpr float TEST_THRESHOLD = 5.0f;

	static constexpr float DEFAULT_PH_1 = 7.0f;
	static constexpr uint16_t DEFAULT_VOLT_1 = 1000;
}

// Test-Fixture für PhSensor
class PhSensorTest : public ::testing::Test
{
protected:
	StrictMock<MockAdConverter> mock_adc;
	StrictMock<MockCalibStorage> mock_calib_storage;
	StrictMock<MockStabilityChecker> mock_stability;

	std::unique_ptr<PhSensor> ph_sensor;

	void SetUp() override
	{
		// Standard-Erwartungen für Konstruktor
		EXPECT_CALL(mock_calib_storage, get_calib_value(StrEq("ph_1"), _))
			.WillOnce(Return(DEFAULT_PH_1));
		EXPECT_CALL(mock_calib_storage, get_calib_raw_value(StrEq("ph_volt_1"), _))
			.WillOnce(Return(DEFAULT_VOLT_1));

		ph_sensor = std::make_unique<PhSensor>(mock_adc, mock_calib_storage, mock_stability);
	}

	void TearDown() override
	{
		ph_sensor.reset();
	}
};

//  ==================== KONSTRUKTOR TESTS ====================

TEST_F(PhSensorTest, Constructor_LoadsCalibrationData)
{
	// Test wird bereits in SetUp() durchgeführt
	// Verifiziert, dass Konstruktor Kalibrierungsdaten lädt
	SUCCEED();
}

TEST_F(PhSensorTest, Constructor_InitializesWithValidParameters)
{
	// Test der Konstruktor-Parameter-Initialisierung
	EXPECT_NE(ph_sensor.get(), nullptr);

	// Verifizierung der geladenen Kalibrierungsdaten
	auto calib = ph_sensor->get_calibration();
	EXPECT_FLOAT_EQ(calib.ph_1, DEFAULT_PH_1);
	EXPECT_EQ(calib.volt_1, DEFAULT_VOLT_1);
}

// ==================== BEGIN METHOD TESTS ====================

TEST_F(PhSensorTest, Begin_SetsAdcChannel)
{
	// Test: begin() setzt den ADC-Kanal korrekt
	ph_sensor->begin(TEST_ADC_CHANNEL);

	// Verifikation durch nachfolgende read_raw() Operation
	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	ph_sensor->read_raw();
	EXPECT_EQ(ph_sensor->get_raw_value(), TEST_VOLTAGE);
}

TEST_F(PhSensorTest, Begin_WithDifferentChannels)
{
	// Test verschiedener ADC-Kanäle
	std::vector<uint8_t> test_channels = {0, 1, 2, 3, 7};

	for (uint8_t channel : test_channels)
	{
		ph_sensor->begin(channel);

		EXPECT_CALL(mock_adc, read_mv(channel))
			.WillOnce(Return(TEST_VOLTAGE));

		ph_sensor->read_raw();
		EXPECT_EQ(ph_sensor->get_raw_value(), TEST_VOLTAGE);
	}
}

// =================== RAW VALUE READING TESTS ==================

TEST_F(PhSensorTest, ReadRaw_ReadsFromCorrectChannel)
{
	ph_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	ph_sensor->read_raw();
	EXPECT_EQ(ph_sensor->get_raw_value(), TEST_VOLTAGE);
}

TEST_F(PhSensorTest, ReadRaw_HandlesMinimumVoltage)
{
	ph_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(0));

	ph_sensor->read_raw();
	EXPECT_EQ(ph_sensor->get_raw_value(), 0);
}

TEST_F(PhSensorTest, ReadRaw_HandlesMaximumVoltage)
{
	ph_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(UINT16_MAX));

	ph_sensor->read_raw();
	EXPECT_EQ(ph_sensor->get_raw_value(), UINT16_MAX);
}

// ========== PH VALUE CALCULATION TESTS ==========

TEST_F(PhSensorTest, CalculateValue_CalculatePhValueCorretly)
{
	ph_sensor->begin(TEST_ADC_CHANNEL);

	// Setze bekannte Werte für Berechnung
	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(DEFAULT_VOLT_1)); // Gleicher Wert wie Kalibrierung

	ph_sensor->read_raw();
	ph_sensor->calculate_value(TEST_TEMPERATURE);

	// Bei gleichen Spannungen sollte pH-Wert dem Kalibrierwert entsprechen
	EXPECT_FLOAT_EQ(ph_sensor->get_value(), DEFAULT_PH_1);
}

TEST_F(PhSensorTest, CalculateValue_TemperatureEffect)
{
	ph_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	ph_sensor->read_raw();
	ph_sensor->calculate_value(TEST_TEMPERATURE);

	float ph_at_25 = ph_sensor->get_value();

	// Neue Messung bei anderer Temperatur
	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	ph_sensor->read_raw();
	ph_sensor->calculate_value(50.0f);

	float ph_at_50 = ph_sensor->get_value();

	// pH-Wert sollte sich bei unterschiedlichen Temperaturen ändern
	EXPECT_NE(ph_at_25, ph_at_50);
}

TEST_F(PhSensorTest, CalculateValue_AbsoluteZeroTemperature)
{
	ph_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	ph_sensor->read_raw();
	ph_sensor->calculate_value(-273.15f); // Absoluter Nullpunkt

	// Bei absoluter Null sollte pH-Wert -1 sein (Fehlerfall)
	EXPECT_EQ(ph_sensor->get_value(), -1.0f);
}

TEST_F(PhSensorTest, CalculateValue_BelowAbsoluteZero)
{
	ph_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	ph_sensor->read_raw();
	ph_sensor->calculate_value(-300.0f); // Unter absoluter Null

	EXPECT_EQ(ph_sensor->get_value(), -1.0f);
}

TEST_F(PhSensorTest, CalculateValue_NearAbsoluteZero)
{
	ph_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	ph_sensor->read_raw();
	ph_sensor->calculate_value(-272.15f); // Knapp über abs. Null

	// Sollte gültigen pH-Wert berechnen (0-14)
	EXPECT_NE(ph_sensor->get_value(), -1.0f);
}

// ==================== STABILITY TESTS ====================

TEST_F(PhSensorTest, CheckStability_CallsStabilityChecker)
{
	EXPECT_CALL(mock_stability, check_stability(TEST_VOLTAGE))
		.Times(1);

	ph_sensor->check_stability(TEST_VOLTAGE);
}

TEST_F(PhSensorTest, IsStable_ReturnsStabilityStatus)
{
	EXPECT_CALL(mock_stability, get_stable_state())
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_TRUE(ph_sensor->is_stable());
	EXPECT_FALSE(ph_sensor->is_stable());
}

TEST_F(PhSensorTest, SetStabilityThreshold_CallsStabilityChecker)
{
	EXPECT_CALL(mock_stability, set_threshold(TEST_THRESHOLD))
		.Times(1);

	ph_sensor->set_stability_threshold(TEST_THRESHOLD);
}

TEST_F(PhSensorTest, GetStabilityThreshold_ReturnsThreshold)
{
	EXPECT_CALL(mock_stability, get_threshold())
		.WillOnce(Return(TEST_THRESHOLD));

	EXPECT_EQ(ph_sensor->get_stability_threshold(), TEST_THRESHOLD);
}

// ==================== CALIBRATION TESTS ====================

TEST_F(PhSensorTest, SetCalibration_UpdatesCalibrationData)
{
	float new_ph_1 = 4.0f;
	uint16_t new_volt_1 = 2000;

	EXPECT_CALL(mock_calib_storage, set_calib_value(StrEq("ph_1"), new_ph_1))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_raw_value(StrEq("ph_volt_1"), new_volt_1))
		.Times(1);

	ph_sensor->set_calibration(new_ph_1, new_volt_1);

	auto calib = ph_sensor->get_calibration();
	EXPECT_EQ(calib.ph_1, new_ph_1);
	EXPECT_EQ(calib.volt_1, new_volt_1);
}

TEST_F(PhSensorTest, GetCalibration_ReturnsCurrentCalibration)
{
	auto calib = ph_sensor->get_calibration();

	EXPECT_EQ(calib.ph_1, DEFAULT_PH_1);
	EXPECT_EQ(calib.volt_1, DEFAULT_VOLT_1);
}

TEST_F(PhSensorTest, SetCalibration_ExtremeValues)
{
	// Test mit extremen aber gültigen Werten
	float extreme_ph = 0.0f;
	uint16_t extreme_volt = UINT16_MAX;

	EXPECT_CALL(mock_calib_storage, set_calib_value(StrEq("ph_1"), extreme_ph))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_raw_value(StrEq("ph_volt_1"), extreme_volt))
		.Times(1);

	ph_sensor->set_calibration(extreme_ph, extreme_volt);

	auto calib = ph_sensor->get_calibration();
	EXPECT_EQ(calib.ph_1, extreme_ph);
	EXPECT_EQ(calib.volt_1, extreme_volt);
}

// ==================== INTEGRATION TESTS ====================

TEST_F(PhSensorTest, CompleteWorkflow_ReadCalculateCheck)
{
	// Simuliert einen kompletten Arbeitsablauf
	ph_sensor->begin(TEST_ADC_CHANNEL);

	// Aktiviert reihenfolgenabhängige Erwartung von EXPECT_CALL's
	InSequence seq;

	// 1. Rohdaten lesen
	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(1200));

	ph_sensor->read_raw();

	// 2. pH-Wert berechnen
	ph_sensor->calculate_value(TEST_TEMPERATURE);

	// 3. Stabilität prüfen
	EXPECT_CALL(mock_stability, check_stability(1200))
		.Times(1);

	ph_sensor->check_stability(ph_sensor->get_raw_value());

	EXPECT_CALL(mock_stability, get_stable_state())
		.WillOnce(Return(true));

	// 4. Ergebnisse validieren
	EXPECT_EQ(ph_sensor->get_raw_value(), 1200);
	EXPECT_NE(ph_sensor->get_value(), -1.0f); // Gültiger pH-Wert
	EXPECT_TRUE(ph_sensor->is_stable());
}

TEST_F(PhSensorTest, MultipleReadings_ConsistentBehavior)
{
	struct TestOP
	{
		uint16_t voltage;
		float ph_value;
	};

	std::vector<TestOP> test_ops = {
		{0, 10.0622f},
		{500, 8.53f},
		{1000, 7.0f},
		{1500, 5.47f},
		{2500, 2.41f},
		{5000, -5.25}};

	ph_sensor->begin(TEST_ADC_CHANNEL);

	for (const auto &op : test_ops)
	{
		EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
			.WillOnce(Return(op.voltage));

		ph_sensor->read_raw();
		ph_sensor->calculate_value(TEST_TEMPERATURE);

		EXPECT_EQ(ph_sensor->get_raw_value(), op.voltage);
		EXPECT_NEAR(ph_sensor->get_value(), op.ph_value, 0.1);
	}
}

// ========== BOUNDARY VALUE TESTS ==========

TEST_F(PhSensorTest, BoundaryValues_VoltageRange)
{
	struct TestOP
	{
		uint16_t voltage;
		float ph_value;
	};

	std::vector<TestOP> test_ops = {
		{0, 10.0622f},
		{1, 10.0592f},
		{UINT16_MAX - 1, -190.6192f},
		{UINT16_MAX, -190.6223f}};

	ph_sensor->begin(TEST_ADC_CHANNEL);

	for (const auto &op : test_ops)
	{
		EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
			.WillOnce(Return(op.voltage));

		ph_sensor->read_raw();
		ph_sensor->calculate_value(TEST_TEMPERATURE);

		EXPECT_EQ(ph_sensor->get_raw_value(), op.voltage);
		EXPECT_NEAR(ph_sensor->get_value(), op.ph_value, 0.1);
	}
}

// ========== ERROR HANDLING TESTS ==========

TEST_F(PhSensorTest, ErrorHandling_InvalidTemperature)
{
	ph_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	ph_sensor->read_raw();

	// Test verschiedener ungültiger Temperaturen
	std::vector<float> invalid_temps = {-400.0f, -273.16f, -1000.0f};

	for (float temp : invalid_temps)
	{
		ph_sensor->calculate_value(temp);
		EXPECT_EQ(ph_sensor->get_value(), -1.0f)
			<< "Failed for temperature: " << temp;
	}
}