// CMPE283 Sample program to test CPUID leaves for assignment.3

#include <stdint.h>
#include <stdio.h>

class CPUID {
  uint32_t regs[4];

public:
  explicit CPUID(unsigned i, unsigned j) {
    asm volatile
      ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
       : "a" (i), "c" (j));
    // ECX is set to zero for CPUID function 4
  }

  const uint32_t &EAX() const {return regs[0];}
  const uint32_t &EBX() const {return regs[1];}
  const uint32_t &ECX() const {return regs[2];}
  const uint32_t &EDX() const {return regs[3];}
};


int main() {

    // VMX exit count
    for (int i = 0; i < 70; i++) {
        CPUID myProc1(0x4ffffffe, i);
        printf("CPUID(0x4FFFFFFE), exit number = %u, number of exits = %u\n", i, myProc1.EAX());
    }

    printf("\n\n");

    // VMX exit duration
    for (int i = 0; i < 70; i++) {
        CPUID myProc2(0x4fffffff, i);
        uint64_t duration = myProc2.EBX();
        duration = myProc2.ECX() + (duration << 32);
        printf("CPUID(0x4FFFFFFF), exit number = %u, time spent: %lu cycles\n", i,  duration);
    }
}
