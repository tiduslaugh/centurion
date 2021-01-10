#include "centurion.hpp"

#include <gtest/gtest.h>

#include "core_mocks.hpp"

class CenturionTest : public testing::Test
{
 protected:
  void SetUp() override
  {
    mocks::reset_core();

    // Sets up expected return values for OK initialization
    constexpr cen::config cfg;
    SDL_Init_fake.return_val = cfg.coreFlags;
    TTF_Init_fake.return_val = 0;
    IMG_Init_fake.return_val = cfg.imageFlags;
    Mix_Init_fake.return_val = cfg.mixerFlags;
    Mix_OpenAudio_fake.return_val = 0;
  }
};

TEST_F(CenturionTest, NoFailureDefaultConfiguration)
{
  try {
    const cen::library library;

    EXPECT_EQ(1, SDL_Init_fake.call_count);
    EXPECT_EQ(1, TTF_Init_fake.call_count);
    EXPECT_EQ(1, IMG_Init_fake.call_count);
    EXPECT_EQ(1, Mix_Init_fake.call_count);

    constexpr cen::config cfg;
    EXPECT_EQ(cfg.coreFlags, SDL_Init_fake.arg0_val);
    EXPECT_EQ(cfg.imageFlags, IMG_Init_fake.arg0_val);
    EXPECT_EQ(cfg.mixerFlags, Mix_Init_fake.arg0_val);

    EXPECT_EQ(cfg.mixerFreq, Mix_OpenAudio_fake.arg0_val);
    EXPECT_EQ(cfg.mixerFormat, Mix_OpenAudio_fake.arg1_val);
    EXPECT_EQ(cfg.mixerChannels, Mix_OpenAudio_fake.arg2_val);
    EXPECT_EQ(cfg.mixerChunkSize, Mix_OpenAudio_fake.arg3_val);
  } catch (...) {
    FAIL();
  }
}

TEST_F(CenturionTest, SDLCoreInitFailure)
{
  SDL_Init_fake.return_val = -1;
  EXPECT_THROW(cen::library{}, cen::sdl_error);
}

TEST_F(CenturionTest, SDLTTFInitFailure)
{
  TTF_Init_fake.return_val = -1;
  EXPECT_THROW(cen::library{}, cen::ttf_error);
}

TEST_F(CenturionTest, SDLImageInitFailure)
{
  IMG_Init_fake.return_val = 0;
  EXPECT_THROW(cen::library{}, cen::img_error);
}

TEST_F(CenturionTest, SDLMixInitFailure)
{
  Mix_Init_fake.return_val = 0;
  EXPECT_THROW(cen::library{}, cen::mix_error);
}

TEST_F(CenturionTest, SDLMixOpenFailure)
{
  Mix_OpenAudio_fake.return_val = -1;
  EXPECT_THROW(cen::library{}, cen::mix_error);
}
