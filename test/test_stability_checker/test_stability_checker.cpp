#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stability_checker.h>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

namespace
{
	static constexpr size_t DEFAULT_SIZE = 100;
	static constexpr float DEFAULT_TRESHOLD = 10.0f;
}

class StabilityCheckerTest : public ::testing::Test
{
protected:
	std::unique_ptr<StabilityChecker> stability_checker;

	void SetUp() override
	{
		stability_checker = nullptr;
	}

	void TearDown() override
	{
		stability_checker.reset();
	}
};

TEST_F(StabilityCheckerTest, Constructor_InitializesWithValidParameters)
{

	stability_checker = std::make_unique<StabilityChecker>(DEFAULT_SIZE, DEFAULT_TRESHOLD);

	EXPECT_NE(stability_checker.get(), nullptr);

	EXPECT_FLOAT_EQ(stability_checker->get_threshold(), DEFAULT_TRESHOLD);
	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 0.0f);
	EXPECT_FLOAT_EQ(stability_checker->get_standard_deviation(), 0.0f);
	EXPECT_FALSE(stability_checker->get_stable_state());
}

TEST_F(StabilityCheckerTest, ThresholdSetterGetter)
{
	stability_checker = std::make_unique<StabilityChecker>(DEFAULT_SIZE, DEFAULT_TRESHOLD);

	stability_checker->set_threshold(15.5f);
	EXPECT_FLOAT_EQ(stability_checker->get_threshold(), 15.5f);

	stability_checker->set_threshold(0.1f);
	EXPECT_FLOAT_EQ(stability_checker->get_threshold(), 0.1f);
}

TEST_F(StabilityCheckerTest, MeanValueCalculationSmallWindow)
{
	stability_checker = std::make_unique<StabilityChecker>(3, 1.0f);

	stability_checker->check_stability(10);
	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 10.0f / 3.0f);

	stability_checker->check_stability(20);
	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 30.0f / 3.0f);

	stability_checker->check_stability(30);
	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 60.0f / 3.0f);
}

TEST_F(StabilityCheckerTest, RingBufferBehavior)
{
	stability_checker = std::make_unique<StabilityChecker>(3, 1.0f);

	// Fülle den Ring-Buffer komplett
	stability_checker->check_stability(10);
	stability_checker->check_stability(20);
	stability_checker->check_stability(30);
	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 20.0f);

	// Überschreibe den ersten Wert
	stability_checker->check_stability(40);
	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 30.0f);

	// Überschreibe den zweiten Wert
	stability_checker->check_stability(50);
	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 40.0f);

	// Überschreibe den dritten Wert
	stability_checker->check_stability(60);
	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 50.0f);
}

TEST_F(StabilityCheckerTest, StandardDeviationIdenticalValues)
{
	stability_checker = std::make_unique<StabilityChecker>(5, 1.0f);

	for (int i = 0; i < 5; i++)
	{
		stability_checker->check_stability(100);
	}

	EXPECT_FLOAT_EQ(stability_checker->get_standard_deviation(), 0.0f);
	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 100.0f);
}

TEST_F(StabilityCheckerTest, StandardDeviationKnownValues)
{
	stability_checker = std::make_unique<StabilityChecker>(5, 10.0f);

	std::vector<uint16_t> raw_values = {13, 16, 20, 18, 15};

	for (uint16_t raw_value : raw_values)
	{
		stability_checker->check_stability(raw_value);
	}

	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 16.4f);
	EXPECT_NEAR(stability_checker->get_standard_deviation(), 2.42, 0.1);
}

TEST_F(StabilityCheckerTest, StabilityDetectionStable)
{
	stability_checker = std::make_unique<StabilityChecker>(5, 2.0f);

	std::vector<uint16_t> raw_values = {100, 101, 99, 100, 100};

	for (uint16_t raw_value : raw_values)
	{
		stability_checker->check_stability(raw_value);
	}

	EXPECT_TRUE(stability_checker->get_stable_state());
	EXPECT_LT(stability_checker->get_standard_deviation(), 2.0f);
}

TEST_F(StabilityCheckerTest, StabilityDetectionUnstable)
{
	stability_checker = std::make_unique<StabilityChecker>(5, 2.0f);

	std::vector<uint16_t> raw_values = {100, 110, 99, 90, 105};

	for (uint16_t raw_value : raw_values)
	{
		stability_checker->check_stability(raw_value);
	}

	EXPECT_FALSE(stability_checker->get_stable_state());
	EXPECT_GT(stability_checker->get_standard_deviation(), 2.0f);
}

TEST_F(StabilityCheckerTest, TransitionUnstableToStable)
{
	stability_checker = std::make_unique<StabilityChecker>(3, 3.0f);

	// Beginne mit instabilen Werten
	stability_checker->check_stability(10);
	stability_checker->check_stability(50);
	stability_checker->check_stability(90);
	EXPECT_FALSE(stability_checker->get_stable_state());

	// Füge stabile Werte hinzu
	stability_checker->check_stability(50);
	stability_checker->check_stability(51);
	stability_checker->check_stability(49);
	EXPECT_TRUE(stability_checker->get_stable_state());
}

TEST_F(StabilityCheckerTest, TransitionStableToUnstable)
{
	stability_checker = std::make_unique<StabilityChecker>(3, 3.0f);

	stability_checker->check_stability(50);
	stability_checker->check_stability(51);
	stability_checker->check_stability(49);
	EXPECT_TRUE(stability_checker->get_stable_state());

	stability_checker->check_stability(90);
	EXPECT_FALSE(stability_checker->get_stable_state());
}

TEST_F(StabilityCheckerTest, LargeValues)
{
	stability_checker = std::make_unique<StabilityChecker>(5, 100.0f);

	std::vector<uint16_t> raw_values = {
		65000, 65100, 64950, 64900, 65150};

	for (uint16_t raw_value : raw_values)
	{
		stability_checker->check_stability(raw_value);
	}

	EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), 65020.0f);
	EXPECT_TRUE(stability_checker->get_stable_state());
	EXPECT_LT(stability_checker->get_standard_deviation(), 100.0f);
}

TEST_F(StabilityCheckerTest, WindowSizeOne)
{
	stability_checker = std::make_unique<StabilityChecker>(1, 1.0f);

	std::vector<uint16_t> raw_values = {54, 86, 32, 12, 500};

	for (uint16_t raw_value : raw_values)
	{
		stability_checker->check_stability(raw_value);

		EXPECT_FLOAT_EQ(stability_checker->get_mean_value(), raw_value);
		EXPECT_FLOAT_EQ(stability_checker->get_standard_deviation(), 0.0f);
		EXPECT_TRUE(stability_checker->get_stable_state());
	}
}

TEST_F(StabilityCheckerTest, ThresholdBoundary)
{
	stability_checker = std::make_unique<StabilityChecker>(3, 5.0f);

	stability_checker->check_stability(10);
	stability_checker->check_stability(15);
	stability_checker->check_stability(20);

	float std_dev = stability_checker->get_standard_deviation();

	stability_checker->set_threshold(std_dev + 0.1f);
	stability_checker->check_stability(10);
	EXPECT_TRUE(stability_checker->get_stable_state());

	stability_checker->set_threshold(std_dev - 0.1f);
	stability_checker->check_stability(20);
	EXPECT_FALSE(stability_checker->get_stable_state());

	// std::cout << "std_dev: " << std_dev << "\n";
}

TEST_F(StabilityCheckerTest, ContinousValueStream)
{
	stability_checker = std::make_unique<StabilityChecker>(5, 3.0f);

	std::vector<uint16_t> values = {100, 102, 98, 101, 99, 103, 97, 100, 101, 99};
	int i = 0;

	for (uint16_t value : values)
	{
		i++;
		stability_checker->check_stability(value);

		EXPECT_GE(stability_checker->get_mean_value(), 0.0f);
		EXPECT_GE(stability_checker->get_standard_deviation(), 0.0f);

		if (i > 4)
		{
			EXPECT_TRUE(stability_checker->get_stable_state());
		}
	}
}