#include <doctest/doctest.h>

#include <reflex/cli.hpp>
#include <testutils.hpp>

#include <string>

using namespace reflex;
using namespace testutils;

namespace
{
struct[[= cli::command{"Dotfiles installer for Mac, Linux, Windows, and WSL"}]] dotfiles
{
  // TODO: change to argument
  static constexpr cli::section section1 = {"INSTALL"};
  [[= cli::option{"-i,--install", "Install (or reinstall) available dotfiles."}
          .flag()]] bool        install  = false;
  [[= cli::option{"-u,--update", "Update from Git before installing."}.flag()]] bool update = false;
  [[= cli::option{
      "-r,--remove <module>",
      "Remove target modules <module>, semicolon-separated."}]] std::string          remove = "";
  [[= cli::option{
      "-R,--repair", "Remove orphaned symlinks and generated files, then re-link/generate."}
          .flag()]] bool                                                             repair = false;
  [[= cli::option{
      "--reset", "Remove all symlinks and generated files managed by this tool. (note: "
                 "copied/merged files will remain)"}
          .flag()]] bool                                                             reset  = false;
  [[= cli::option{
      "--uninstall", "Deletes symlinks, logs, and backups, then deletes install directory."}
          .flag()]] bool uninstall                                                          = false;

  // options
  // maintenance
  static constexpr cli::section section2 = {"MAINTENANCE"};
  [[= cli::option{"--clean [N]", "Clean backups and reset both log files."}]] int clean = 0;
  [[= cli::option{"--clean-logs", "Clean both log files."}.flag()]] bool clean_logs     = false;
  [[= cli::option{
      "--clean-backups [N]", "Clean backups. N: remove backup dirs older than N days. (default "
                             "when omitted: keep the latest "
                             "DOTFILES_MAX_BACKUPS dirs)"}]] int         clean_backups  = 0;

  static constexpr cli::section section3 = {"INFORMATION"};
  [[= cli::option{"--help", "Print this message and exit."}.flag()]] bool      help    = false;
  static constexpr std::string_view                                            version = "v0.1.0";
  [[= cli::option{"-l,--list", "Display all available modules."}.flag()]] bool list    = false;

  // install modules
  static constexpr cli::section section4 = {"INSTALL MODULES"};
  [[= cli::option{
      "-I,--include <module...>",
      "Install ONLY the specified modules <module>, semicolon-separated."}]] std::string include =
      "";
  [[= cli::option{
      "-x,--exclude <module...>",
      "Install all modules EXCEPT specified <module>, semicolon-separated."}]] std::string exclude =
      "";

  // behaviour
  static constexpr cli::section section5    = {"BEHAVIOUR"};
  [[= cli::option{"-n,--dry-run", "Print planned actions without modifying the disk."}
          .flag()]] bool        dry_run     = false;
  [[= cli::option{"-K,--autorestart", "Automatically restart shell at script end."}
          .flag()]] bool        autorestart = false;
  [[= cli::option{"--no-regen", "Do not regenerate relevant files if they already exist."}
          .flag()]] bool        no_regen    = false;
  [[= cli::option{"--no-backup", "Do not backup pre-existing copied files."}
          .flag()]] bool        no_backup   = false;
  [[= cli::option{"-U,--interactive", "Prompt for confirmation before every action/modification."}
          .flag()]] bool        interactive = false;
  [[= cli::option{"--noconfirm", "Do not prompt for any confirmation."}.flag()]] bool noconfirm =
      false;
  [[= cli::option{"-y,--yes", "Auto-accept yes to prompts. Alias to --noconfirm."}
          .flag()]] bool yes = false;
  [[= cli::option{"--force", "Overwrite existing files/links without backup."}.flag()]] bool force =
      false;

  // logging
  static constexpr cli::section section6 = {"LOGGING"};
  [[= cli::option{"-d,--debug", "Print debug output (useful for diagnosing)."}.flag()]] bool debug =
      false;
  [[= cli::option{"-q,--quiet", "Suppress all standard output except errors."}.flag()]] bool quiet =
      false;
  /* FIXME: enum*/ [[= cli::option{
      "--log-level <level>", "Set log verbosity <level>. Valid options are: trace, debug, info, "
                             "notice, warn, or error. (default: info)"}]] int log_level = 2;
  [[= cli::option{"--no-log", "Disable logging to stdout (still writes to file)."}
          .flag()]] bool                                                      no_log    = false;
  [[= cli::option{"--time", "Print total execution time at program finish."}.flag()]] bool time =
      false;

  //===============================================================================================================
  // environment variables
  //===============================================================================================================
  // Names come from the members: dotfiles_log_dir -> DOTFILES_LOG_DIR. The
  // first env slot overrides the name; empty derives it.
  [[= cli::env{"", "Path to store log files. (default: ~/.config/dotfiles/logs)"}]] cli::env_var<
      std::filesystem::path>
      dotfiles_log_dir = {"~/.config/dotfiles/logs"};
  [[= cli::env{"", "Path to store temporary cache. (default: ~/.cache/dotfiles)"}]] cli::env_var<
      std::filesystem::path>
      dotfiles_cache_dir = {"~/.cache/dotfiles"};
  [[= cli::env{
      "", "Set to 0 or false to disable log file writing. (default: 1)"}]] cli::env_var<bool>
      dotfiles_log = {true};
  [[= cli::env{
      "", "Set to 1 or true to install with uncommitted local changes. (default: 0)"}]] cli::
      env_var<bool>
          dotfiles_local_mods = {false};
  [[= cli::env{
      "", "Set to 1 or true to restart shell at script finish. (default: 0)"}]] cli::env_var<bool>
      dotfiles_autorestart = {false};
  [[= cli::env{
      "",
      "Maximum number of backups to keep. Set to 0 to keep all. (default: 10)"}]] cli::env_var<int>
      dotfiles_max_backups = {10};

  int operator()() const
  {
    return 0;
  }
};

struct[[= cli::command{"A command that declares none of the built-ins itself."}]] bare
{
  static constexpr cli::section                                  retired = {"RETIRED"};
  static constexpr cli::section                                  flags   = {"FLAGS"};
  [[= cli::option{"-f,--flag", "A declared flag."}.flag()]] bool flag    = false;

  int operator()() const
  {
    return 0;
  }
};

struct[[= cli::command{"No markers, so no titles."}]] flat
{
  [[= cli::option{"-a,--alpha", "First."}.flag()]] bool alpha = false;

  int operator()() const
  {
    return 0;
  }
};

auto usage_of(auto&& cmd, const char** argv, int argc)
{
  int  rc  = -1;
  auto out = capture_out_err([&] { rc = cli::run(cmd, argc, argv); }).first;
  return std::pair{rc, out};
}
} // namespace

TEST_CASE("reflex::cli: section markers title the options declared under them")
{
  const char* argv[]   = {"dotfiles", "--help"};
  const auto [rc, out] = usage_of(dotfiles{}, argv, 2);
  CHECK_EQ(rc, 0);

  const auto options = out.find("OPTIONS:");
  const auto install = out.find("\nINSTALL\n");
  const auto maint   = out.find("\nMAINTENANCE\n");
  const auto info    = out.find("\nINFORMATION\n");
  const auto modules = out.find("\nINSTALL MODULES\n");
  const auto behav   = out.find("\nBEHAVIOUR\n");
  const auto logging = out.find("\nLOGGING\n");

  CHECK_NE(options, std::string::npos);
  CHECK(options < install);
  CHECK(install < maint);
  CHECK(maint < info);
  CHECK(info < modules);
  CHECK(modules < behav);
  CHECK(behav < logging);

  CHECK_LT(install, out.find("Install (or reinstall) available dotfiles."));
  CHECK_LT(out.find("Deletes symlinks, logs, and backups"), maint);
  CHECK_LT(maint, out.find("Clean both log files."));

  CHECK_LT(info, out.find("Print this message and exit."));
  CHECK_LT(out.find("Print this message and exit."), out.find("Print the version and exit."));
  CHECK_LT(out.find("Print the version and exit."), out.find("Display all available modules."));
  CHECK_LT(out.find("Display all available modules."), modules);

  // environment variables keep their own untitled block after the options
  CHECK_NE(out.find("ENVIRONMENT VARIABLES"), std::string::npos);
  CHECK_LT(logging, out.find("ENVIRONMENT VARIABLES"));
  CHECK_NE(out.find("DOTFILES_LOG_DIR"), std::string::npos);
}

TEST_CASE("reflex::cli: appended built-ins join the last section")
{
  const char* argv[]   = {"bare", "--help"};
  const auto [rc, out] = usage_of(bare{}, argv, 2);
  CHECK_EQ(rc, 0);

  CHECK_EQ(out.find("FLAGS"), out.rfind("FLAGS"));
  CHECK_LT(out.find("\nFLAGS\n"), out.find("-f, --flag"));
  CHECK_LT(out.find("-f, --flag"), out.find("--help"));

  CHECK_EQ(out.find("RETIRED"), std::string::npos);
}

TEST_CASE("reflex::cli: a command without markers prints no section titles")
{
  const char* argv[]   = {"flat", "--help"};
  const auto [rc, out] = usage_of(flat{}, argv, 2);
  CHECK_EQ(rc, 0);

  CHECK_NE(out.find("OPTIONS:\n  -a, --alpha"), std::string::npos);
}
