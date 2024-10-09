#pragma once

namespace HandleldTerminal {
namespace Constants {

const char ACTION_ID[] = "HandleldTerminal.Action";
const char MENU_ID[] = "HandleldTerminal.Menu";

namespace Simulator {

const char NAME[] = "Name";
const char ABI[] = "Abi";
const char MEMORY[] = "Memory";
const char CPUNUMBER[] = "CpuNumber";
const char X_RESOLUTION[] = "X.Resolution";
const char Y_RESOLUTION[] = "Y.Resolution";

namespace Linux {
const char NAME[] = "Simulator.Name";
const char ABI[] = "Simulator.Abi";
const char SSH_PORT[] = "Simulator.Ssh.Port";
const char USERNAME[] = "Simulator.UserName";
const char PASSWORD[] = "Simulator.Password";
const char MEMORY[] = "Simulator.Memory";
const char CPUNUMBER[] = "Simulator.CpuNumber";
const char X_RESOLUTION[] = "Simulator.X.Resolution";
const char Y_RESOLUTION[] = "Simulator.Y.Resolution";
}
namespace Android {
const char NAME[] = "Simulator.Name";
const char ABI[] = "Simulator.Abi";
const char MEMORY[] = "sdcard.size";
const char CPUNUMBER[] = "hw.cpu.ncore";
const char X_RESOLUTION[] = "hw.lcd.width";
const char Y_RESOLUTION[] = "hw.lcd.height";
}
}
} // namespace HandleldTerminal
} // namespace Constants
