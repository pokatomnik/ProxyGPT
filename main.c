#include <errno.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

static const char *const CHATGPT_PATH =
    "/Applications/ChatGPT.app/Contents/MacOS/ChatGPT";
static const char *const PROXY = "socks5h://127.0.0.1:1080";

int main(int argc, char *argv[]) {
  (void)argc;

  if (setenv("ALL_PROXY", PROXY, 1) == -1 ||
      setenv("all_proxy", PROXY, 1) == -1 ||
      setenv("NO_PROXY", "localhost,127.0.0.1,::1", 1) == -1 ||
      setenv("no_proxy", "localhost,127.0.0.1,::1", 1) == -1 ||
      setenv("NODE_USE_ENV_PROXY", "1", 1) == -1) {
    perror("setenv");
    return EXIT_FAILURE;
  }

  // Запускаем ChatGPT отдельным процессом. Сам launcher должен завершиться
  // сразу, иначе его процесс будет жить всё время работы ChatGPT.
  argv[0] = (char *)CHATGPT_PATH;

  pid_t chatgpt_pid;
  int status =
      posix_spawn(&chatgpt_pid, CHATGPT_PATH, NULL, NULL, argv, environ);
  if (status != 0) {
    errno = status;
    perror(CHATGPT_PATH);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
