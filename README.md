# ProxyGPT

Небольшой macOS launcher для запуска ChatGPT через локальный SOCKS5-прокси.

Программа запускает бинарник ChatGPT по адресу:

```text
/Applications/ChatGPT.app/Contents/MacOS/ChatGPT
```

Оригинальное приложение ChatGPT не изменяется.

## Что делает launcher

Перед запуском ChatGPT программа устанавливает следующие переменные окружения:

```text
ALL_PROXY=socks5h://127.0.0.1:1080
all_proxy=socks5h://127.0.0.1:1080
NO_PROXY=localhost,127.0.0.1,::1
no_proxy=localhost,127.0.0.1,::1
NODE_USE_ENV_PROXY=1
```

После этого ChatGPT запускается отдельным процессом, а launcher сразу завершается. Благодаря этому `ProxyGPT.app` не удерживается в Dock. Все аргументы командной строки, переданные launcher-у, также передаются ChatGPT.

Локальный SOCKS5-прокси должен быть доступен на `127.0.0.1:1080`.

## Структура проекта

```text
.
├── main.c
├── Makefile
└── ProxyGPT.app
    └── Contents
        ├── Info.plist
        └── MacOS
            └── launcher
```

- `main.c` — исходный код launcher-а.
- `Makefile` — сборка бинарника.
- `ProxyGPT.app` — macOS application bundle.
- `ProxyGPT.app/Contents/MacOS/launcher` — собранный исполняемый файл.

## Сборка

Для сборки нужен установленный Xcode Command Line Tools, включая `clang` и `make`.

Из корня проекта выполните:

```sh
make
```

Бинарник будет скомпилирован непосредственно в:

```text
ProxyGPT.app/Contents/MacOS/launcher
```

При необходимости принудительно пересобрать и перезаписать бинарник можно использовать:

```sh
make -B
```

Удаление собранного бинарника:

```sh
make clean
```

## Запуск

После сборки приложение можно запустить из корня проекта:

```sh
open ProxyGPT.app
```

Также bundle можно переместить в `~/Applications` или `/Applications`, а затем запускать через Finder, Launchpad или Dock.

## Изменение пути и прокси

Путь к ChatGPT и адрес прокси заданы в `main.c` константами:

```c
static const char *const CHATGPT_PATH =
    "/Applications/ChatGPT.app/Contents/MacOS/ChatGPT";
static const char *const PROXY = "socks5h://127.0.0.1:1080";
```

Если ChatGPT установлен в другом месте или прокси использует другой адрес, измените эти значения и пересоберите приложение командой:

```sh
make -B
```

## Подпись приложения

Для локального использования можно создать ad-hoc подпись:

```sh
codesign --force --sign - ProxyGPT.app
```

Если macOS блокирует запуск после скачивания или копирования bundle, проверьте сообщение системы и при необходимости разрешите запуск в настройках безопасности.

## Важно

Переменные окружения устанавливаются только для нового процесса ChatGPT. Если ChatGPT уже запущен и использует существующий экземпляр, он может не получить новые значения proxy-переменных. В таком случае полностью завершите ChatGPT и запустите `ProxyGPT.app` снова.
