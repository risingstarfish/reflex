#include <doctest/doctest.h>

#include <reflex/cli.hpp>
#include <testutils.hpp>

#include <sstream>
#include <string>

using namespace reflex;
using namespace testutils;

namespace
{
struct[[= cli::command{"Every switch spelling a usage line can hold."}]] spellings
{
  [[= cli::option{"-b", "A short spelling on its own."}.flag()]] //
      bool brief = false;

  [[= cli::option{"--verbose", "A long spelling on its own."}.flag()]] //
      bool verbose = false;

  [[= cli::option{"-p,--pair", "Both spellings."}.flag()]] //
      bool pair = false;

  int operator()() const
  {
    return 0;
  }
};

struct[[= cli::command{"Help too long for a single row."}]] longhand
{
  [[= cli::option{
      "--log-level <level>", "Set log verbosity <level>. Valid options are: trace, debug, info, "
                             "notice, warn, or error."}]] int level = 0;

  int operator()() const
  {
    return 0;
  }
};

template <cli::configuration config> auto usage_of_width(const char** argv, int argc)
{
  int  rc = -1;
  auto out =
      capture_out_err([&] { rc = cli::run<longhand, config>(longhand{}, argc, argv); }).first;
  return std::pair{rc, out};
}

// The column a row's help text starts at — where its continuations belong.
std::size_t help_col(const std::string& out, std::string_view row)
{
  auto stream = std::istringstream{out};
  for(std::string line; std::getline(stream, line);)
  {
    if(line.starts_with(row))
    {
      return line.find_first_not_of(' ', row.size());
    }
  }
  return std::string::npos;
}

std::size_t widest_line(const std::string& out)
{
  auto        stream = std::istringstream{out};
  std::size_t widest = 0;
  for(std::string line; std::getline(stream, line);)
  {
    if(line.size() > widest)
    {
      widest = line.size();
    }
  }
  return widest;
}

auto usage_of(const char** argv, int argc)
{
  int  rc  = -1;
  auto out = capture_out_err([&] { rc = cli::run(spellings{}, argc, argv); }).first;
  return std::pair{rc, out};
}

bool has_row(const std::string& out, std::string_view switches, std::string_view help)
{
  auto       ss   = std::istringstream{out};
  const auto head = "  " + std::string{switches} + " ";
  for(std::string line; std::getline(ss, line);)
  {
    if(line.starts_with(head) and line.ends_with(help))
    {
      return true;
    }
  }
  return false;
}
} // namespace

TEST_CASE("reflex::cli: usage names the program, not the path it was found at")
{
  const char* argv[]   = {"/opt/somewhere/spellings", "--help"};
  const auto [rc, out] = usage_of(argv, 2);
  CHECK_EQ(rc, 0);
  CHECK_NE(out.find("USAGE: spellings"), std::string::npos);
  CHECK_EQ(out.find("/opt/somewhere"), std::string::npos);
}

TEST_CASE("reflex::cli: a bare name is left alone")
{
  const char* argv[]   = {"spellings", "--help"};
  const auto [rc, out] = usage_of(argv, 2);
  CHECK_EQ(rc, 0);
  CHECK_NE(out.find("USAGE: spellings"), std::string::npos);
}

TEST_CASE("reflex::cli: each switch spelling gets its own usage row")
{
  const char* argv[]   = {"spellings", "--help"};
  const auto [rc, out] = usage_of(argv, 2);
  CHECK_EQ(rc, 0);
  CHECK(has_row(out, "-b", "A short spelling on its own."));
  CHECK(has_row(out, "--verbose", "A long spelling on its own."));
  CHECK(has_row(out, "-p, --pair", "Both spellings."));
}

TEST_CASE("reflex::cli: the configured width folds help into what the name column leaves")
{
  const char* argv[]   = {"longhand", "--help"};
  const auto [rc, out] = usage_of_width<cli::configuration{.max_width = 60}>(argv, 2);
  CHECK_EQ(rc, 0);

  // The sentence is longer than the terminal leaves beside the switch, so it
  // continues on the next row rather than running off the right edge.
  CHECK(has_row(out, "--log-level <level>", "Set log verbosity <level>. Valid"));

  // A continuation lands under the help above it, not a space further in.
  const auto col = help_col(out, "  --log-level <level>");
  CHECK_NE(col, std::string::npos);
  CHECK_NE(out.find("\n" + std::string(col, ' ') + "options are: trace, debug, info,"),
           std::string::npos);
  CHECK_LE(widest_line(out), 60);
}

TEST_CASE("reflex::cli: help folds at the default width when the command names none")
{
  const char* argv[]   = {"longhand", "--help"};
  const auto [rc, out] = usage_of_width<cli::configuration{}>(argv, 2);
  CHECK_EQ(rc, 0);

  CHECK_LE(widest_line(out), cli::default_max_width);
  CHECK_NE(out.find("Set log verbosity <level>. Valid options are:"), std::string::npos);
}

TEST_CASE("reflex::cli: a width of zero leaves help unfolded")
{
  const char* argv[]   = {"longhand", "--help"};
  const auto [rc, out] = usage_of_width<cli::configuration{.max_width = 0}>(argv, 2);
  CHECK_EQ(rc, 0);

  CHECK(has_row(
      out,
      "--log-level <level>",
      "Set log verbosity <level>. Valid options are: trace, debug, info, notice, warn, or error."));
  CHECK_GT(widest_line(out), cli::default_max_width);
}
