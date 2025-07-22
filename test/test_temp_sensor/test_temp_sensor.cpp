#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <temp_sensor.h>
#include <../mocks/mock_ad_converter.h>
#include <../mocks/mock_calib_storage.h>
#include <../mocks/mock_stability_checker.h>

using testing::_;
using testing::InSequence;
using testing::Return;
using testing::StrEq;
using testing::StrictMock;

namespace
{
	static constexpr uint8_t TEST_ADC_CHANNEL = 0;
	static constexpr uint16_t TEST_VOLTAGE = 1500;
	static constexpr float TEST_THRESHOLD = 5.0f;

	static constexpr float DEFAULT_TEMP_1 = 25.0f;
	static constexpr uint16_t DEFAULT_VOLT_1 = 1000;
	static constexpr float DEFAULT_TEMP_2 = 50.0f;
	static constexpr uint16_t DEFAULT_VOLT_2 = 2000;
}

// Test-Fixture für TempSensor
class TempSensorTest : public ::testing::Test
{
protected:
	StrictMock<MockAdConverter> mock_adc;
	StrictMock<MockCalibStorage> mock_calib_storage;
	StrictMock<MockStabilityChecker> mock_stability;

	std::unique_ptr<TempSensor> temp_sensor;

	void SetUp() override
	{
		// Standard-Verhalten für Calibration Storage Mock definieren
		EXPECT_CALL(mock_calib_storage, get_calib_value(StrEq("temp_1"), _))
			.WillOnce(Return(DEFAULT_TEMP_1));
		EXPECT_CALL(mock_calib_storage, get_calib_raw_value(StrEq("temp_volt_1"), _))
			.WillOnce(Return(DEFAULT_VOLT_1));
		EXPECT_CALL(mock_calib_storage, get_calib_value(StrEq("temp_2"), _))
			.WillOnce(Return(DEFAULT_TEMP_2));
		EXPECT_CALL(mock_calib_storage, get_calib_raw_value(StrEq("temp_volt_2"), _))
			.WillOnce(Return(DEFAULT_VOLT_2));

		temp_sensor = std::make_unique<TempSensor>(mock_adc, mock_calib_storage, mock_stability);
	}

	void TearDown() override
	{
		temp_sensor.reset();
	}
};

// ==================== KONSTRUKTOR TESTS ====================

TEST_F(TempSensorTest, Constructor_LoadsCalibrationData)
{
	// Test wird bereits in SetUp() durchgeführt
	// Verifiziert, dass Konstruktor Kalibrierungsdaten lädt
	SUCCEED();
}

TEST_F(TempSensorTest, Constructor_InitializesWithValidParameters)
{
	// Test der Konstruktor-Parameter-Initialisierung
	EXPECT_NE(temp_sensor.get(), nullptr);

	// Verifizierung der geladenen Kalibrierungsdaten
	auto calib = temp_sensor->get_calibration();
	EXPECT_FLOAT_EQ(calib.temp_1, DEFAULT_TEMP_1);
	EXPECT_EQ(calib.volt_1, DEFAULT_VOLT_1);
	EXPECT_FLOAT_EQ(calib.temp_2, DEFAULT_TEMP_2);
	EXPECT_EQ(calib.volt_2, DEFAULT_VOLT_2);
}

// ==================== BEGIN METHOD TESTS ====================
TEST_F(TempSensorTest, Begin_SetsAdcChannel)
{
	// Test: begin() setzt den ADC-Kanal korrekt
	temp_sensor->begin(TEST_ADC_CHANNEL);

	// Verifikation durch nachfolgende read_raw() Operation
	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	temp_sensor->read_raw();
	EXPECT_EQ(temp_sensor->get_raw_value(), TEST_VOLTAGE);
}

TEST_F(TempSensorTest, Begin_WithDifferentChannels)
{
	// Test verschiedener ADC-Kanäle
	std::vector<uint8_t> test_channels = {0, 1, 2, 3, 7};

	for (uint8_t channel : test_channels)
	{
		temp_sensor->begin(channel);

		EXPECT_CALL(mock_adc, read_mv(channel))
			.WillOnce(Return(TEST_VOLTAGE));

		temp_sensor->read_raw();
		EXPECT_EQ(temp_sensor->get_raw_value(), TEST_VOLTAGE);
	}
}

// =================== RAW VALUE READING TESTS ==================
TEST_F(TempSensorTest, ReadRaw_ReadsFromCorrectChannel)
{
	temp_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	temp_sensor->read_raw();
	EXPECT_EQ(temp_sensor->get_raw_value(), TEST_VOLTAGE);
}

TEST_F(TempSensorTest, ReadRaw_HandlesMinimumVoltage)
{
	temp_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(0));

	temp_sensor->read_raw();
	EXPECT_EQ(temp_sensor->get_raw_value(), 0);
}

TEST_F(TempSensorTest, ReadRaw_HandlesMaximumVoltage)
{
	temp_sensor->begin(TEST_ADC_CHANNEL);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(UINT16_MAX));

	temp_sensor->read_raw();
	EXPECT_EQ(temp_sensor->get_raw_value(), UINT16_MAX);
}

// =========== TEMP VALUE CALCULATION TESTS ===========

TEST_F(TempSensorTest, CalculateValue_CalculateTemperatureCorrectly)
{
	temp_sensor->begin(TEST_ADC_CHANNEL);

	float expected_temp = DEFAULT_TEMP_1 + ((DEFAULT_TEMP_2 - DEFAULT_TEMP_1) / (DEFAULT_VOLT_2 - DEFAULT_VOLT_1)) * (TEST_VOLTAGE - DEFAULT_VOLT_1);

	// Setze bekannte Werte für Berechnung
	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	temp_sensor->read_raw();
	temp_sensor->calculate_value();

	EXPECT_FLOAT_EQ(temp_sensor->get_value(), expected_temp);
}

TEST_F(TempSensorTest, CalculateValue_LowerBoundary_ReturnTemp1)
{
	temp_sensor->begin(TEST_ADC_CHANNEL);

	float expected_temp = 25.0f;

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(DEFAULT_VOLT_1));

	temp_sensor->read_raw();
	temp_sensor->calculate_value();

	EXPECT_FLOAT_EQ(temp_sensor->get_value(), expected_temp);
}

TEST_F(TempSensorTest, CalculateValue_UpperBoundary_ReturnTemp2)
{
	// Arrange

	temp_sensor->begin(TEST_ADC_CHANNEL);

	uint16_t test_voltage = 2000; // Entspricht temp_2
	float expected_temp = 50.0f;

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(test_voltage));

	// Act
	temp_sensor->read_raw();
	temp_sensor->calculate_value();

	// Assert
	EXPECT_FLOAT_EQ(temp_sensor->get_value(), expected_temp);
}

// ==================== STABILITY TESTS ====================
TEST_F(TempSensorTest, CheckStability_CallsStabilityChecker)
{
	EXPECT_CALL(mock_stability, check_stability(TEST_VOLTAGE))
		.Times(1);

	temp_sensor->check_stability(TEST_VOLTAGE);
}

TEST_F(TempSensorTest, IsStable_ReturnsStabilityStatus)
{
	EXPECT_CALL(mock_stability, get_stable_state())
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_TRUE(temp_sensor->is_stable());
	EXPECT_FALSE(temp_sensor->is_stable());
}

TEST_F(TempSensorTest, SetStabilityThreshold_CallsStabilityChecker)
{
	EXPECT_CALL(mock_stability, set_threshold(TEST_THRESHOLD))
		.Times(1);

	temp_sensor->set_stability_threshold(TEST_THRESHOLD);
}

TEST_F(TempSensorTest, GetStabilityThreshold_ReturnsThreshold)
{
	EXPECT_CALL(mock_stability, get_threshold())
		.WillOnce(Return(TEST_THRESHOLD));

	EXPECT_FLOAT_EQ(temp_sensor->get_stability_threshold(), TEST_THRESHOLD);
}

// ==================== CALIBRATION TESTS ====================
TEST_F(TempSensorTest, SetCalibration_UpdatesCalibrationData)
{
	float new_temp_1 = 20.0f, new_temp_2 = 60.0f;
	uint16_t new_volt_1 = 800, new_volt_2 = 2400;

	EXPECT_CALL(mock_calib_storage, set_calib_value(StrEq("temp_1"), new_temp_1))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_raw_value(StrEq("temp_volt_1"), new_volt_1))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_value(StrEq("temp_2"), new_temp_2))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_raw_value(StrEq("temp_volt_2"), new_volt_2))
		.Times(1);

	temp_sensor->set_calibration(new_temp_1, new_volt_1, new_temp_2, new_volt_2);

	auto calib_data = temp_sensor->get_calibration();
	EXPECT_FLOAT_EQ(calib_data.temp_1, new_temp_1);
	EXPECT_EQ(calib_data.volt_1, new_volt_1);
	EXPECT_FLOAT_EQ(calib_data.temp_2, new_temp_2);
	EXPECT_EQ(calib_data.volt_2, new_volt_2);
}

TEST_F(TempSensorTest, GetCalibration_ReturnsCurrentCalibrationData)
{
	auto calib_data = temp_sensor->get_calibration();

	EXPECT_FLOAT_EQ(calib_data.temp_1, DEFAULT_TEMP_1);
	EXPECT_EQ(calib_data.volt_1, DEFAULT_VOLT_1);
	EXPECT_FLOAT_EQ(calib_data.temp_2, DEFAULT_TEMP_2);
	EXPECT_EQ(calib_data.volt_2, DEFAULT_VOLT_2);
}

TEST_F(TempSensorTest, SetCalibration_ExtremeValues)
{
	float extreme_temp_1 = 0.0f, extreme_temp_2 = 0.0f;
	uint16_t extreme_volt_1 = UINT16_MAX, extreme_volt_2 = UINT16_MAX;

	EXPECT_CALL(mock_calib_storage, set_calib_value(StrEq("temp_1"), extreme_temp_1))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_raw_value(StrEq("temp_volt_1"), extreme_volt_1))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_value(StrEq("temp_2"), extreme_temp_2))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_raw_value(StrEq("temp_volt_2"), extreme_volt_2))
		.Times(1);

	temp_sensor->set_calibration(extreme_temp_1, extreme_volt_1, extreme_temp_2, extreme_volt_2);

	auto calib_data = temp_sensor->get_calibration();
	EXPECT_FLOAT_EQ(calib_data.temp_1, extreme_temp_1);
	EXPECT_EQ(calib_data.volt_1, extreme_volt_1);
	EXPECT_FLOAT_EQ(calib_data.temp_2, extreme_temp_2);
	EXPECT_EQ(calib_data.volt_2, extreme_volt_2);
}

// ==================== INTEGRATION TESTS ====================
TEST_F(TempSensorTest, CompleteWorkflow_ReadCalculateCheck)
{
	temp_sensor->begin(TEST_ADC_CHANNEL);

	InSequence seq;

	float expected_temp = 37.5f;

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(TEST_VOLTAGE));

	temp_sensor->read_raw();

	temp_sensor->calculate_value();

	EXPECT_CALL(mock_stability, check_stability(TEST_VOLTAGE))
		.Times(1);

	temp_sensor->check_stability(temp_sensor->get_raw_value());

	EXPECT_CALL(mock_stability, get_stable_state())
		.WillOnce(Return(true));

	EXPECT_EQ(temp_sensor->get_raw_value(), TEST_VOLTAGE);
	EXPECT_FLOAT_EQ(temp_sensor->get_value(), expected_temp);
	EXPECT_TRUE(temp_sensor->is_stable());
}

TEST_F(TempSensorTest, MultipleReadings_ConsistentBehavior)
{
	struct TestOP
	{
		uint16_t voltage;
		float temperature;
	};

	std::vector<TestOP> test_ops = {
		{0, 0.0f},
		{500, 12.5f},
		{1000, 25.0f},
		{1500, 37.5f},
		{2500, 62.5f},
		{5000, 125.0f}};

	temp_sensor->begin(TEST_ADC_CHANNEL);

	for (const auto &op : test_ops)
	{
		EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
			.WillOnce(Return(op.voltage));

		temp_sensor->read_raw();
		temp_sensor->calculate_value();

		EXPECT_EQ(temp_sensor->get_raw_value(), op.voltage);
		EXPECT_NEAR(temp_sensor->get_value(), op.temperature, 0.025);
	}
}

// ========== BOUNDARY VALUE TESTS ==========

TEST_F(TempSensorTest, BoundaryValues_VoltageRange)
{
	struct TestOP
	{
		uint16_t voltage;
		float temperature;
	};

	std::vector<TestOP> test_ops = {
		{0, 0.0f},
		{1, 0.025f},
		{UINT16_MAX - 1, 1638.35f},
		{UINT16_MAX, 1638.375f}};

	temp_sensor->begin(TEST_ADC_CHANNEL);

	for (const auto &op : test_ops)
	{
		EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
			.WillOnce(Return(op.voltage));

		temp_sensor->read_raw();
		temp_sensor->calculate_value();

		EXPECT_EQ(temp_sensor->get_raw_value(), op.voltage);
		EXPECT_NEAR(temp_sensor->get_value(), op.temperature, 0.025);
	}
}

/*
// ==================== EDGE CASE TESTS ====================
TEST_F(TempSensorTest, CalculateValue_ZeroVoltageDifference_ShouldHandleGracefully)
{
	uint16_t same_volt_1_2 = 1000;

	EXPECT_CALL(mock_calib_storage, set_calib_value(StrEq("temp_1"), DEFAULT_TEMP_1))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_raw_value(StrEq("temp_volt_1"), same_volt_1_2))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_value(StrEq("temp_2"), DEFAULT_TEMP_2))
		.Times(1);
	EXPECT_CALL(mock_calib_storage, set_calib_raw_value(StrEq("temp_volt_2"), same_volt_1_2))
		.Times(1);

	temp_sensor->set_calibration(DEFAULT_TEMP_1, same_volt_1_2, DEFAULT_TEMP_2, same_volt_1_2);

	EXPECT_CALL(mock_adc, read_mv(TEST_ADC_CHANNEL))
		.WillOnce(Return(1500));

	auto calib_data = temp_sensor->get_calibration();

	EXPECT_EQ(calib_data.volt_1, 1000);
	EXPECT_EQ(calib_data.volt_2, 1000);
	EXPECT_EQ(calib_data.temp_1, 25.0f);
	EXPECT_EQ(calib_data.temp_2, 50.0f);

	// Act & Assert
	temp_sensor->read_raw();
	// Bei Division durch Null sollte der Code robust reagieren
	// (In der aktuellen Implementierung würde dies zu undefiniertem Verhalten führen)
	EXPECT_NO_THROW(temp_sensor->calculate_value());
	EXPECT_FLOAT_EQ(temp_sensor->get_value(), 37.5f) << "Failed Test";
}
*/