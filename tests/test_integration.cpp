#include "../src/VoleMachine.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifndef VOLE_PROGRAMS_DIR
#define VOLE_PROGRAMS_DIR "programs"
#endif

namespace {

using Bytes = std::vector<std::uint8_t>;

int failures = 0;

void check(bool condition, const std::string& label) {
    if (!condition) {
        std::cerr << "  FAIL: " << label << '\n';
        ++failures;
    }
}

std::string program(const std::string& name) {
    return std::string(VOLE_PROGRAMS_DIR) + "/" + name;
}

std::string tempProgram(const std::string& name, const std::string& text) {
    const std::string path =
        (std::filesystem::temp_directory_path() / name).string();
    std::ofstream(path) << text;
    return path;
}

void testLoadPlacesWordsBigEndian() {
    Vole::VoleMachine m;
    const Vole::LoadResult r = m.loadProgram(program("program1.vol"), 0x00);
    check(r.ok, "program1.vol loads");
    check(m.memory().read(0x00) == 0x20 && m.memory().read(0x01) == 0x05,
          "first word 2005 is stored high byte first");
    check(m.memory().read(0x0E) == 0xC0 && m.memory().read(0x0F) == 0x00,
          "last word C000 is stored at 0E/0F");
    check(m.cpu().getPc() == 0x00, "PC points at the load address");
}

void testLoadAtOtherAddress() {
    Vole::VoleMachine m;
    check(m.loadProgram(program("program1.vol"), 0x10).ok, "loads at 10");
    check(m.cpu().getPc() == 0x10, "PC is set to 10");
    check(m.memory().read(0x10) == 0x20, "first byte lands at 10");
    check(m.memory().read(0x00) == 0x00, "nothing is written below 10");
}

void testLoadClearsOldState() {
    Vole::VoleMachine m;
    m.setRegister(3, 0x99);
    m.setMemory(0x80, 0x55);
    check(m.loadProgram(program("program1.vol"), 0x00).ok, "loads");
    check(m.cpu().getRegisters().get(3) == 0x00, "load clears registers");
    check(m.memory().read(0x80) == 0x00, "load clears memory");
}

void testFailedLoadKeepsState() {
    Vole::VoleMachine m;
    m.setRegister(5, 0x77);
    m.setMemory(0x00, 0xBB);

    check(!m.loadProgram(program("does_not_exist.vol"), 0x00).ok,
          "missing file is rejected");
    check(!m.loadProgram(program("program1.vol"), 0x01).ok,
          "odd address is rejected");
    check(!m.loadProgram(program("program1.vol"), 0xFA).ok,
          "program that does not fit is rejected");
    const std::string bad = tempProgram("vole_bad_opcode.vol", "2048\nD123\n");
    check(!m.loadProgram(bad, 0x00).ok, "invalid opcode is rejected");

    check(m.cpu().getRegisters().get(5) == 0x77, "registers kept after failed loads");
    check(m.memory().read(0x00) == 0xBB, "memory kept after failed loads");
}

void testProgram1PrintsOF() {
    Vole::VoleMachine m;
    check(m.loadProgram(program("program1.vol"), 0x00).ok, "program1 loads");
    const StepResult r = m.run(1000);
    check(r.status == StepStatus::HALTED, "program1 halts");
    check(m.cpu().isHalted(), "CPU reports halted");
    check(m.cpu().getPc() == 0x10, "HALT leaves PC two bytes past C000");
    check(m.screen().getBytes() == Bytes{0x0F}, "program1 prints 0F");
    check(m.memory().read(0x00) == 0x0F, "program1 stored 0F at address 00");
}

void testProgram2PrintsOO() {
    Vole::VoleMachine m;
    check(m.loadProgram(program("program2.vol"), 0x00).ok, "program2 loads");
    const StepResult r = m.run(1000);
    check(r.status == StepStatus::HALTED, "program2 halts");
    check(m.screen().getBytes() == Bytes{0x00}, "program2 prints 00");
}

void testSingleStep() {
    Vole::VoleMachine m;
    check(m.loadProgram(program("program1.vol"), 0x00).ok, "program1 loads");
    check(m.step().status == StepStatus::EXECUTED, "first step executes");
    check(m.cpu().getPc() == 0x02, "step advances PC by 2");
    check(m.cpu().getRegisters().get(0) == 0x05, "2005 loads 05 into R0");
    check(m.cpu().getIr() == 0x2005, "IR holds the full 16-bit word 2005");
}

void testStepLimit() {
    Vole::VoleMachine m;
    const std::string loop = tempProgram("vole_loop.vol", "B000\n");
    check(m.loadProgram(loop, 0x00).ok, "loop program loads");
    check(m.run(50).status == StepStatus::STEP_LIMIT, "endless loop hits the limit");
    check(!m.cpu().isHalted(), "step limit does not halt the CPU");
}

void testFetchErrorAtFF() {
    Vole::VoleMachine m;
    const std::string jump = tempProgram("vole_jump_ff.vol", "B0FF\n");
    check(m.loadProgram(jump, 0x00).ok, "jump program loads");
    check(m.step().status == StepStatus::EXECUTED, "jump executes");
    check(m.cpu().getPc() == 0xFF, "PC is FF");
    check(m.step().status == StepStatus::FETCH_ERROR, "fetch at FF is an error");
}

void testEditedMemoryIsRevalidated() {
    Vole::VoleMachine m;
    check(m.loadProgram(program("program1.vol"), 0x00).ok, "loads");
    m.setMemory(0x00, 0xD0);
    check(m.step().status == StepStatus::INVALID_INSTRUCTION,
          "an instruction edited into opcode D is rejected at fetch");
}

void testFFPort() {
    Vole::VoleMachine m;
    const std::string path =
        tempProgram("vole_ff.vol", "10FF\n30FF\nC000\n");  // R0=mem[FF]; screen<-R0
    check(m.loadProgram(path, 0x00).ok, "ff program loads");
    m.setMemory(0xFF, 0xAB);
    check(m.screen().getBytes().empty(), "setMemory(FF) writes RAM, not the screen");
    m.run(100);
    check(m.cpu().getRegisters().get(0) == 0xAB, "CPU load from FF reads RAM");
    check(m.screen().getBytes() == Bytes{0xAB}, "CPU store to FF writes the screen");
    check(m.memory().read(0xFF) == 0xAB, "RAM at FF is untouched by the store");
}

void testLoadIntoFFHasNoScreenOutput() {
    Vole::VoleMachine m;
    const std::string path = tempProgram("vole_fe.vol", "C000\n");
    check(m.loadProgram(path, 0xFE).ok, "one word fits at FE");
    check(m.memory().read(0xFE) == 0xC0 && m.memory().read(0xFF) == 0x00,
          "word occupies FE and FF");
    check(m.screen().getBytes().empty(), "loading produces no screen output");
}

void testReset() {
    Vole::VoleMachine m;
    m.loadProgram(program("program1.vol"), 0x00);
    m.run(1000);
    m.reset();
    check(m.cpu().getPc() == 0x00, "reset clears PC");
    check(!m.cpu().isHalted(), "reset clears halted");
    check(m.cpu().getRegisters().get(0) == 0x00, "reset clears registers");
    check(m.memory().read(0x00) == 0x00, "reset clears memory");
    check(m.screen().getBytes().empty(), "reset clears the screen");
}

void testReloadAfterHalt() {
    Vole::VoleMachine m;
    m.loadProgram(program("program1.vol"), 0x00);
    m.run(1000);
    check(m.loadProgram(program("program2.vol"), 0x00).ok, "reload works");
    check(!m.cpu().isHalted(), "reload clears the halted flag");
    check(m.screen().getBytes().empty(), "reload clears old screen output");
    m.run(1000);
    check(m.screen().getBytes() == Bytes{0x00}, "second program runs");
}


// Runs a small program from a string and returns the machine's final result.
StepResult runText(Vole::VoleMachine& m, const std::string& name,
                   const std::string& text) {
    const Vole::LoadResult r = m.loadProgram(tempProgram(name, text), 0x00);
    check(r.ok, name + " loads");
    return m.run(10000);
}

void testBitwiseAndRotate() {
    Vole::VoleMachine m;
    // R0=F0 R1=3C; R2=R0|R1  R3=R0&R1  R4=R0^R1; print them; rotate R0 right 4
    const StepResult r = runText(m, "vole_bitwise.vol",
        "20F0\n213C\n7201\n8301\n9401\n32FF\n33FF\n34FF\nA004\n30FF\nC000\n");
    check(r.status == StepStatus::HALTED, "bitwise program halts");
    check((m.screen().getBytes() == Bytes{0xFC, 0x30, 0xCC, 0x0F}),
          "OR/AND/XOR/rotate print FC 30 CC 0F");
}

void testJumpLoop() {
    Vole::VoleMachine m;
    // Counts 3 -> 0 with two's-complement add and a conditional jump.
    const StepResult r = runText(m, "vole_loop_count.vol",
        "2003\n21FF\n2200\n5001\n30FF\nB20E\nB006\nC000\n");
    check(r.status == StepStatus::HALTED, "countdown halts");
    check((m.screen().getBytes() == Bytes{0x02, 0x01, 0x00}),
          "countdown prints 02 01 00");
}

void testLoadStoreMove() {
    Vole::VoleMachine m;
    // R0=5A; mem[80]=R0; R1=mem[80]; R2=R1 (40RS copies R into S); print R2
    const StepResult r = runText(m, "vole_move.vol",
        "205A\n3080\n1180\n4012\n32FF\nC000\n");
    check(r.status == StepStatus::HALTED, "move program halts");
    check(m.memory().read(0x80) == 0x5A, "opcode 3 stores the register value");
    check(m.cpu().getRegisters().get(1) == 0x5A, "opcode 1 loads from memory");
    check(m.cpu().getRegisters().get(2) == 0x5A, "opcode 4 copies R into S");
    check(m.screen().getBytes() == Bytes{0x5A}, "result reaches the screen");
}

void testFloatAdd() {
    Vole::VoleMachine m;
    const StepResult r = runText(m, "vole_float.vol",
        "2048\n2148\n6201\n32FF\nC000\n");  // 0x48 + 0x48 (0.5 + 0.5)
    check(r.status == StepStatus::HALTED, "float program halts");
    check(m.screen().getBytes() == Bytes{0x58}, "float 48 + 48 = 58 (1.0)");
}

void testOddPcJump() {
    Vole::VoleMachine m;
    check(m.loadProgram(tempProgram("vole_odd.vol", "B005\n"), 0x00).ok,
          "odd-jump program loads");
    m.setMemory(0x05, 0xC0);
    m.setMemory(0x06, 0x00);
    check(m.step().status == StepStatus::EXECUTED, "jump executes");
    check(m.cpu().getPc() == 0x05, "odd PC is legal");
    check(m.step().status == StepStatus::HALTED, "halt at odd PC");
    check(m.cpu().getPc() == 0x07, "halt leaves PC two past itself");
}

void testHaltedMachineStaysHalted() {
    Vole::VoleMachine m;
    m.loadProgram(program("program1.vol"), 0x00);
    m.run(1000);
    check(m.step().status == StepStatus::HALTED, "step on a halted CPU is HALTED");
    check(m.run(10).status == StepStatus::HALTED, "run on a halted CPU is HALTED");
    check(m.screen().getBytes() == Bytes{0x0F}, "no extra output after halt");
}

void testRunWithZeroSteps() {
    Vole::VoleMachine m;
    m.loadProgram(program("program1.vol"), 0x00);
    check(m.run(0).status == StepStatus::STEP_LIMIT, "run(0) returns the limit");
    check(m.cpu().getPc() == 0x00, "run(0) executes nothing");
}

void testSetters() {
    Vole::VoleMachine m;
    m.setRegister(0xF, 0xEE);
    m.setMemory(0x80, 0x12);
    check(m.cpu().getRegisters().get(0xF) == 0xEE, "setRegister reaches the CPU");
    check(m.memory().read(0x80) == 0x12, "setMemory reaches RAM");
}

#define RUN(test)                                                         \
    do {                                                                  \
        const int before = failures;                                      \
        test();                                                           \
        std::cout << (failures == before ? "[ OK ]   " : "[FAILED] ")     \
                  << #test << '\n';                                       \
    } while (0)

}  // namespace

int main() {
    RUN(testLoadPlacesWordsBigEndian);
    RUN(testLoadAtOtherAddress);
    RUN(testLoadClearsOldState);
    RUN(testFailedLoadKeepsState);
    RUN(testProgram1PrintsOF);
    RUN(testProgram2PrintsOO);
    RUN(testSingleStep);
    RUN(testStepLimit);
    RUN(testFetchErrorAtFF);
    RUN(testEditedMemoryIsRevalidated);
    RUN(testFFPort);
    RUN(testLoadIntoFFHasNoScreenOutput);
    RUN(testReset);
    RUN(testReloadAfterHalt);
    RUN(testBitwiseAndRotate);
    RUN(testJumpLoop);
    RUN(testLoadStoreMove);
    RUN(testFloatAdd);
    RUN(testOddPcJump);
    RUN(testHaltedMachineStaysHalted);
    RUN(testRunWithZeroSteps);
    RUN(testSetters);

    if (failures == 0) {
        std::cout << "All integration tests passed.\n";
        return 0;
    }
    std::cerr << failures << " integration check(s) failed.\n";
    return 1;
}
