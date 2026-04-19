# Bare-metal RISC-V Emulator

RV32IM 명령어를 해석하는 베어메탈 RISC-V 에뮬레이터입니다. 호스트는 C++로 작성되어 있고, SDL2를 사용해 화면 출력과 키보드/게임패드 입력을 처리합니다. 게스트 프로그램은 운영체제 없이 `0x00000000`에서 시작하는 RISC-V raw binary로 로드되며, UART, framebuffer, timer, keyboard 같은 장치는 MMIO로 접근합니다.

## 개발 배경

처음에는 듀얼쇼크 입력을 SDL2로 받아 버튼별 비트를 출력하고 시각화하는 작은 프로젝트로 시작했습니다. SDL2가 로우레벨 하드웨어 접근을 직접 하지 않고도 입력/출력 자원을 다룰 수 있다는 점을 확인한 뒤, 이를 호스트 장치로 사용해 작은 가상 머신을 만들어 보기로 방향을 확장했습니다.

초기 목표는 다음 순서였습니다.

1. RISC-V VM에서 `Hello World`를 UART 콘솔에 출력한다.
2. SDL2 창에 색칠된 사각형을 띄워 framebuffer MMIO를 검증한다.
3. CPU 루프와 화면 갱신을 최적화해 초당 명령어 처리량을 확보한다.
4. 자체 런타임을 보강해 `doomgeneric`을 실행한다.

## 현재 구현

- RV32I 기본 정수 명령어 구현
  - R/I/S/B/U/J 타입 명령어
  - `LUI`, `AUIPC`, `JAL`, `JALR`, branch, load/store, 산술/논리 연산
- RV32M 확장 구현
  - `MUL`, `MULH`, `MULHSU`, `MULHU`
  - `DIV`, `DIVU`, `REM`, `REMU`
- 16 MiB guest RAM
- little-endian load/store 처리
- signed/unsigned load의 부호 확장 처리
- MMIO 기반 UART, framebuffer, timer, keyboard 입력
- SDL2 기반 host emulator
  - 640x400 XRGB8888 framebuffer를 SDL texture로 표시
  - 키보드 입력과 SDL GameController 입력을 guest keyboard queue로 전달
- freestanding guest runtime
  - `_start`에서 stack pointer 설정
  - `.bss` 직접 초기화
  - `main()` 호출 후 무한 루프 진입
- guest용 최소 libc 구현
  - `printf`, `snprintf`, `vsnprintf`
  - `memcpy`, `memset`, `strlen`, `strcmp`, `strstr` 등 문자열/메모리 함수
  - `malloc`, `calloc`, `realloc`, `free`
  - WAD 파일 접근을 위한 메모리 기반 `fopen`, `fread`, `fseek`, `ftell`

## 프로젝트 구조

```text
host/
  include/cpu.h          host CPU 클래스 인터페이스
  source/cpu.cpp         RV32IM fetch/decode/execute와 MMIO 구현
  source/emulator.cpp    SDL2 window/input loop와 guest binary loader

guest/
  include/               guest용 libc/mmio 헤더
  source/                guest startup code와 최소 libc 구현
  linker/test.ld         bare-metal guest linker script
  user_program/          DOOMGeneric 등 외부 guest program 위치

```

## 메모리 맵

| 주소 | 이름 | 설명 |
| --- | --- | --- |
| `0x00000000` | Guest entry | guest binary가 로드되고 `_start`가 위치하는 시작 주소 |
| `0x00000000` - `0x00ffffff` | RAM | 16 MiB guest RAM |
| `0x00800000` | WAD load address | host가 선택적으로 WAD 파일을 올리는 위치 |
| `0x01000000` | Stack top | guest stack pointer 초기값 |
| `0x10000000` | UART | 하위 8비트를 문자로 출력 |
| `0x20000000` | Framebuffer | 32-bit XRGB8888 framebuffer |
| `0x30000000` | Timer | 부팅 이후 경과 시간(ms) 읽기 |
| `0x40000000` | Key value | 입력 queue에서 key event 하나 읽기 |
| `0x40000004` | Key status | 입력 queue가 비어 있지 않으면 1 |

`guest/linker/test.ld`는 code, rodata, data, bss, heap, stack의 위치를 명시합니다. 운영체제가 없기 때문에 ELF를 OS가 적절한 위치에 올려주는 과정이 없고, 링커 스크립트로 guest 메모리 배치를 직접 고정해야 합니다.

## MMIO 인터페이스

게스트 코드는 `guest/include/mmio.h`의 매크로로 장치에 접근합니다.

```c
#define UART_ADDR   ((volatile uint32_t*)0x10000000)
#define FRAMEBUFFER ((volatile uint32_t*)0x20000000)
#define TIMER_MS    (*(volatile uint32_t*)0x30000000)
#define KEY_VALUE   (*(volatile uint32_t*)0x40000000)
#define KEY_STATUS  (*(volatile uint32_t*)0x40000004)
```

키 입력은 host SDL event를 guest queue에 넣는 방식입니다. 값의 하위 8비트는 key code이고, bit 8은 press/release 상태입니다.

```text
pressed event  = (1 << 8) | key
released event = (0 << 8) | key
```

## 빌드 환경

Windows + MSYS2 환경을 기준으로 개발했습니다.

필요한 도구:

- MSYS2 UCRT64 또는 MINGW64 toolchain
- SDL2 개발 패키지
- `riscv32-unknown-elf-gcc`
- `riscv32-unknown-elf-objcopy`
- `riscv32-unknown-elf-objdump`
- `riscv32-unknown-elf-nm`

PowerShell에서 MSYS2 toolchain 경로를 먼저 잡아둡니다.

```powershell
$env:PATH='C:\msys64\ucrt64\bin;C:\msys64\mingw64\bin;' + $env:PATH
New-Item -ItemType Directory -Force build | Out-Null
```

## Host emulator 빌드

`host/source/emulator.cpp`는 현재 DOOM key mapping을 위해 `doomkeys.h`를 include합니다. 따라서 `doomgeneric` 소스가 `guest/user_program/doomgeneric/doomgeneric` 아래에 있어야 합니다.

```powershell
g++ `
  -std=c++17 `
  -O2 `
  -Wall `
  -Wextra `
  -I host/include `
  -I guest/user_program/doomgeneric/doomgeneric `
  host/source/emulator.cpp `
  host/source/cpu.cpp `
  -o build/emulator.exe `
  -lmingw32 `
  -lSDL2main `
  -lSDL2
```

## Guest 프로그램 빌드

일반 guest 프로그램은 freestanding RISC-V C 코드로 빌드한 뒤 raw binary로 변환해서 emulator에 넘깁니다.

예시:

```powershell
riscv32-unknown-elf-gcc `
  -march=rv32im `
  -mabi=ilp32 `
  -O1 `
  -ffreestanding `
  -nostdlib `
  -nostartfiles `
  "-Wl,--no-relax" `
  "-Wl,-Map=build/screen_test.map" `
  -T guest/linker/test.ld `
  -I guest/include `
  guest/source/start.c `
  guest/source/stdio.c `
  guest/source/stdlib.c `
  guest/source/string.c `
  guest/source/ctype.c `
  guest/source/math.c `
  guest/source/errno.c `
  test/screen_test.c `
  -lgcc `
  -o build/screen_test.elf

riscv32-unknown-elf-objcopy -O binary build/screen_test.elf build/screen_test.bin
```

실행:

```powershell
.\build\emulator.exe .\build\screen_test.bin 200000
```

세 번째 인자는 SDL 프레임마다 실행할 CPU step 수입니다.

## DOOMGeneric 빌드와 실행
<table>
<tr>
  <td> <img title="mainScreen" src="img\mainScreen.png" width="400", height="200"> </td>
  <td> <img title="gameScreen" src="img\gameScreen.png" width="400", height="200"> </td>
</tr>
</table>

`user_program_build/Makefile`은 `doomgeneric`을 guest binary로 빌드하기 위한 설정입니다. 현재 저장소에서는 `guest/user_program/*`가
제외되어 있습니다. DoomGeneric 저장소스 코드와 wadfile을 직접 배치하여애 실행이 가능합니다

기대하는 경로:

```text
guest/user_program/doomgeneric/doomgeneric
guest/user_program/doom_platform/doomgeneric_RISCV.c
```

빌드:

```powershell
make -C user_program_build
```

실행:

```powershell
.\build\emulator.exe .\build\doomgeneric.bin .\path\to\doom1.wad 2000
```

host는 WAD 파일을 `0x00800000`에 미리 적재합니다. guest의 `fopen`, `fread`, `fseek`, `ftell`은 실제 파일 시스템을 사용하지 않고 이 메모리 영역을 파일처럼 읽습니다.

## 트러블슈팅

### Immediate 인코딩

RISC-V의 J-type, B-type, S-type immediate는 instruction word 안에서 순서대로 배치되어 있지 않습니다. 처음에는 상위 비트를 단순히 잘라 쓰는 방식으로 접근했지만, 실제 인코딩에 맞춰 각 비트 조각을 분리한 뒤 다시 조립하도록 수정했습니다.

### PC 업데이트

현재 `fetch()`는 instruction을 읽은 직후 `pc += 4`를 수행합니다. 따라서 branch와 jump는 이미 증가한 PC가 아니라 현재 instruction 주소를 기준으로 계산해야 합니다. 이 문제를 해결하기 위해 `JAL`, branch 등에서는 `(pc - 4) + offset` 형태로 target을 계산합니다.

### Load/store와 부호 확장

메모리는 `uint8_t` 배열입니다. 16비트, 32비트 값을 읽을 때는 little-endian 순서로 조립해야 하고, `LB`, `LH`는 signed extension, `LBU`, `LHU`는 zero extension을 적용해야 합니다. 이 과정에서 8비트 값을 shift하기 전에 32비트로 캐스팅하지 않으면 overflow가 날 수 있습니다.

### 링커 스크립트와 런타임

운영체제가 없는 환경에서는 시작 주소, section 배치, stack pointer, bss 초기화를 직접 처리해야 합니다. `guest/linker/test.ld`에서 `_start`를 entry로 고정하고, `guest/source/start.c`에서 stack 설정과 `.bss` zero-fill을 수행한 뒤 `main()`을 호출합니다.

### libc 의존성 제거

DOOMGeneric은 일반적인 C 라이브러리 함수와 파일 입출력을 사용합니다. 하지만 guest에는 OS와 파일 시스템이 없으므로 필요한 libc 함수를 직접 구현했습니다. 파일 관련 함수는 실제 디스크 접근 대신 host가 메모리에 올려둔 WAD 영역을 읽도록 구성했습니다.

### RV32M과 libgcc

`printf` 계열 구현과 DOOMGeneric 코드에는 나눗셈, 곱셈, 일부 floating-point helper가 필요합니다. CPU에는 RV32M 곱셈/나눗셈 명령어를 구현했고, 빌드 시 `-lgcc`를 링크해 컴파일러가 생성하는 helper 함수 의존성을 처리합니다.


## 현재 한계와 TODO

- CPU, memory, bus, MMIO가 아직 `RISCV_CPU` 클래스에 함께 들어 있어 구조 분리가 필요합니다.
- 실제 파일 시스템은 없습니다. 현재 파일 I/O는 WAD 메모리 영역을 읽는 용도에 맞춰져 있습니다.
- `malloc`은 단순 bump allocator에 가깝고, `free`는 실질적인 반환을 수행하지 않습니다.
- exception, interrupt, privilege level, CSR은 구현되어 있지 않습니다.
- framebuffer MMIO 범위 검사와 guest 오동작에 대한 방어 로직은 더 보강할 필요가 있습니다.
- `doomgeneric` 소스와 WAD 파일은 저장소에 포함하지 않습니다. 사용자가 별도로 준비해야 합니다.
