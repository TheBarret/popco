# PopCo Monitor

<img width="650" height="504" alt="image" src="https://github.com/user-attachments/assets/72c37380-4073-406e-9480-fb2563805541" />

*Small System monitor project in Fallout RobCo theme.*

| File | Function |
|---|---|
| `main.c` | window/renderer setup, event loop, ties everything together |
| `config.h/.c` | loads/saves `popco.conf` (plain `key=value`, no deps) |
| `stats.h/.c` | samples CPU/memory/disk/uptime/hostname from `/proc`, `sysinfo()`, `statvfs()` |
| `render.h/.c` | text, progress bars, and the full panel layout |
| `effects.h/.c` | bloom, scanlines, vignette, flicker/roll bar, chromatic aberration |

## Config (`popco.conf`)

Auto-generates a new file if absent, controls the colors and theme settings.  
(reloading config by pressing **R**)  

## Custom Fonts

The app has a few fonts defined that can be changed in the source/settings.  

**Fonts: ShareTechMono-Regular.ttf comes with its own license file**  

```sh
sudo apt install libsdl2-dev libsdl2-ttf-dev
make
./monitor
```

