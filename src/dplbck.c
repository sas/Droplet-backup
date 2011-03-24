#include <stdio.h>
#include <string.h>

#include <commands/backup/backup.h>
#include <commands/help/help.h>
#include <commands/list/list.h>
#include <commands/restore/restore.h>
#include <commands/stats/stats.h>
#include <usage.h>

int main(int argc, char *argv[])
{
  struct {
    char *cmd_name;
    int (*cmd)(int, char *[]);
  } commands[] = {
    { "backup",   cmd_backup, },
    { "restore",  cmd_restore, },
    { "list",     cmd_list, },
    { "stats",    cmd_stats, },
    { "help",     cmd_help, },
  };

  --argc; ++argv;

  /* XXX: We skip the arguments for now. */
  while (argc && *argv[0] == '-')
    --argc, ++argv;

  if (argc == 0)
  {
    usage(stderr);
    return 1;
  }

  for (unsigned int i = 0; i < sizeof (commands) / sizeof (commands[0]); ++i)
    if (strcmp(*argv, commands[i].cmd_name) == 0)
      return commands[i].cmd(argc, argv);

  /* We never reach this point if there is a valid command. */
  err(EXIT_USAGE_FAIL, "unknown command: %s\n", *argv);
}
