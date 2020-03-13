#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <storages/redis/impl/base.hpp>

namespace redis {

struct GeoaddArg {
  double lon;
  double lat;
  std::string member;
};

struct GeoradiusOptions {
  enum class Sort { kNone, kAsc, kDesc };

  bool withcoord = false;
  bool withdist = false;
  bool withhash = false;
  size_t count = 0;
  Sort sort = Sort::kNone;
};

struct SetOptions {
  enum class Exist { kSetAlways, kSetIfNotExist, kSetIfExist };

  int seconds = 0;
  int milliseconds = 0;
  Exist exist = Exist::kSetAlways;
};

struct ZaddOptions {
  enum class Exist { kAddAlways, kAddIfNotExist, kAddIfExist };
  enum class ReturnValue { kAddedCount, kChangedCount };

  ZaddOptions() = default;
  ZaddOptions(Exist exist, ReturnValue return_value = ReturnValue::kAddedCount)
      : exist(exist), return_value(return_value) {}
  ZaddOptions(ReturnValue return_value, Exist exist = Exist::kAddAlways)
      : exist(exist), return_value(return_value) {}

  Exist exist = Exist::kAddAlways;
  ReturnValue return_value = ReturnValue::kAddedCount;
};

ZaddOptions operator|(ZaddOptions::Exist exist,
                      ZaddOptions::ReturnValue return_value);
ZaddOptions operator|(ZaddOptions::ReturnValue return_value,
                      ZaddOptions::Exist exist);

struct ScanOptions {
  boost::optional<ScanCursor> cursor;
  boost::optional<std::string> pattern;
  boost::optional<size_t> count;
};

struct ScoreOptions {
  bool withscores = false;
};

struct RangeOptions {
  boost::optional<size_t> offset;
  boost::optional<size_t> count;
};

struct RangeScoreOptions {
  ScoreOptions score_options;
  RangeOptions range_options;
};

void PutArg(CmdArgs::CmdArgsArray& args_, const GeoaddArg& arg);

void PutArg(CmdArgs::CmdArgsArray& args_, const std::vector<GeoaddArg>& arg);

void PutArg(CmdArgs::CmdArgsArray& args_, const GeoradiusOptions& arg);

void PutArg(CmdArgs::CmdArgsArray& args_, const SetOptions& arg);

void PutArg(CmdArgs::CmdArgsArray& args_, const ZaddOptions& arg);

void PutArg(CmdArgs::CmdArgsArray& args_, const ScanOptions& arg);

void PutArg(CmdArgs::CmdArgsArray& args_, const ScoreOptions& arg);

void PutArg(CmdArgs::CmdArgsArray& args_, const RangeOptions& arg);

void PutArg(CmdArgs::CmdArgsArray& args_, const RangeScoreOptions& arg);

}  // namespace redis
