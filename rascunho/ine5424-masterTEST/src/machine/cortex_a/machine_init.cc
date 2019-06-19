// EPOS Cortex-A Mediator Initialization

#include <machine.h>

__BEGIN_SYS

void Machine::pre_init(System_Info *si)
{
    //===
    if (si->bm.n_cpus > Traits<Machine>::CPUS)
        si->bm.n_cpus = Traits<Machine>::CPUS;

    // Multicore conditional start up
    int cpu_id = Machine::cpu_id();

    db<Setup>(TRC) << "PC_Setup(bi=" << reinterpret_cast<void *>(bi) << ",sp=" << reinterpret_cast<void *>(CPU::sp()) << ")" << endl;

    db<Setup>(INF) << "System_Info=" << *si << endl;

    if (cpu_id == 0)
    { // Boot strap CPU (BSP)

        // Disable hardware interrupt triggering at PIC
        i8259A::reset();

        unsigned int memb, memt;
        detect_memory(&memb, &memt);

        // Calibrate timers
        calibrate_timers();

        // Build the memory model
        build_lm();
        build_pmm();

        // Try to obtain a node id for this machine
        get_node_id();

        // Print basic facts about this EPOS instance
        say_hi();

        // Configure the memory model defined above
        setup_idt();
        setup_gdt();
        setup_sys_pt();
        setup_sys_pd();

        // Enable paging
        // We won't be able to print anything before the remap() bellow
        db<Setup>(INF) << "IP=" << CPU::ip() << endl;
        db<Setup>(INF) << "SP=" << reinterpret_cast<void *>(CPU::sp()) << endl;
        db<Setup>(INF) << "CR0=" << reinterpret_cast<void *>(CPU::cr0()) << endl;
        db<Setup>(INF) << "CR3=" << reinterpret_cast<void *>(CPU::cr3()) << endl;

        enable_paging();

        // Adjust pointers that will still be used to their logical addresses
        bi = reinterpret_cast<char *>(unsigned(bi) | PHY_MEM);
        si = reinterpret_cast<System_Info *>(SYS_INFO);
        VGA::init(Memory_Map::VGA); // Display can be Serial_Display, so VGA here!
        APIC::remap(Memory_Map::APIC);

        // Configure a TSS for system calls and inter-level interrupt handling
        setup_tss();

        // Load EPOS parts (e.g. INIT, SYSTEM, APP)
        load_parts();

        // Signalize other CPUs that paging is up
        Paging_Ready = true;
    }
    else
    { // Additional CPUs (APs)

        // Wait for the Boot CPU to setup page tables
        while (!Paging_Ready)
            ;

        enable_paging();
        setup_tss();
    }
    Machine::smp_barrier(si->bm.n_cpus);

    db<Setup>(INF) << "IP=" << CPU::ip() << endl;
    db<Setup>(INF) << "SP=" << reinterpret_cast<void *>(CPU::sp()) << endl;
    db<Setup>(INF) << "CR0=" << reinterpret_cast<void *>(CPU::cr0()) << endl;
    db<Setup>(INF) << "CR3=" << reinterpret_cast<void *>(CPU::cr3()) << endl;

    // SETUP ends here, transfer control to next stage (INIT or APP)
    call_next();

    // SETUP is now part of the free memory and this point should never be
    // reached, but, just in case ... :-)
    panic();
    Machine_Model::pre_init();

    Display::init();
}
//===
Machine::smp_barrier(si->bm.n_cpus);

//END

void Machine::send_sgi(int intID, int targetCPU, int filter)
{
    int *sgi_reg = (int *)(0x1f000000 + 0x1F00);
    *sgi_reg = (filter << 24) | ((1 << targetCPU) << 16) | (intID);
}

void Machine::init()
{
    db<Init, Machine>(TRC) << "Machine::init()" << endl;
    //===============
    // for (int i = 0; i <= 2; i++)
    Machine::smp_barrier();
    // if (Machine::cpu_id() == 0)
    // {
    send_sgi(0x0, 0x0f, 0x01); //DEU UM SGI NO CPU1
    // int *apAddr = (int *)0x10000030; // SYS_FLAGSSET register
    // *apAddr = (int)0x10000;          // all APs execute from 0x10000
    // }

    Machine_Model::init();

    if (Traits<IC>::enabled)
        IC::init();
    if (Traits<Timer>::enabled)
        Timer::init();
#ifdef __USB_H
    if (Traits<USB>::enabled)
        USB::init();
#endif
}

__END_SYS
