import sys
from random import randint
from dataclasses import dataclass

pageFaults = 0
dirty_writes = 0
disk_reference = 0


@dataclass
class MemoryAction:
    processNumber: int
    page: int
    offset: int
    action: str


@dataclass
class Page:
    def __init__(self):
        self.data: list(int) = [int] * 512


@dataclass
class MainMemPage:
    def __init__(self):
        self.data: list()
        self.mem_slot: int
        self.dirty_bit: bool
        self.reference_bit: bool
        self.age: int
        self.process: int = sys.maxsize
        self.page: int = sys.maxsize


@dataclass
class Process:
    def __init__(self):
        self.pageTable: list(Page) = [Page() for i in range(128)]


def replace_random() -> int:
    return randint(0, 31)


def replace_fifo(main_memory: list[MainMemPage]) -> int:
    return sorted(main_memory, key=lambda mem: mem.age)[0].mem_slot


def replace_lru(main_memory: list[MainMemPage]) -> int:
    temp: list[MainMemPage] = sorted(main_memory, key=lambda mem: mem.age)
    if temp[0].age == temp[1].age:
        if temp[0].dirty_bit and temp[1].dirty_bit is False: return temp[1].mem_slot
        elif temp[0].dirty_bit is False and temp[1].dirty_bit: return temp[0].mem_slot
        else:
            if temp[0].mem_slot < temp[1].mem_slot: return temp[0].mem_slot
            else: return temp[1].mem_slot
    return temp[0].mem_slot


def replace_per(main_memory: list[MainMemPage]) -> int:
    for mem in main_memory:
        if not mem.reference_bit and not mem.dirty_bit: return mem.mem_slot
    for mem in main_memory:
        if not mem.reference_bit and mem.dirty_bit: return mem.mem_slot
    for mem in main_memory:
        if mem.reference_bit and not mem.dirty_bit: return mem.mem_slot
    for mem in main_memory:
        if mem.reference_bit and mem.dirty_bit: return mem.mem_slot
    return 0


def replace_bel(main_memory: list[MainMemPage], current_index: int, lines: list) -> int:
    memSlots = [x for x in range(32)]
    for i in range(current_index, len(lines)):
        process_num, mem_location, action = [int(value) if value.isnumeric() else value for value in lines[i].split()]
        mem_action = MemoryAction(process_num, mem_location >> 9, mem_location & 0b1111111, action)
        for j in range(len(main_memory)):
            if main_memory[j].process == mem_action.processNumber and main_memory[j].page == mem_action.page:
                if j in memSlots: memSlots.remove(j)
        if len(memSlots) == 1: return memSlots[0]
    return memSlots[0]


def remove_page(main_memory: list[MainMemPage], processes: list[Process], page_to_remove):
    # Check if we need to write any changes back to memory
    if main_memory[page_to_remove].dirty_bit:
        # When there's a dirty bit, it accesses the disk
        global dirty_writes, disk_reference
        dirty_writes += 1
        disk_reference += 1
        processes[main_memory[page_to_remove].process-1].pageTable[main_memory[page_to_remove].page] = main_memory[page_to_remove].page
    return


def add_page(main_memory: list[MainMemPage], processes: list[Process], open_page: int, mem_action: MemoryAction, age: int):
    main_memory[open_page].data = processes[mem_action.processNumber-1].pageTable[mem_action.page]
    main_memory[open_page].mem_slot = open_page
    main_memory[open_page].process = mem_action.processNumber
    main_memory[open_page].page = mem_action.page
    main_memory[open_page].dirty_bit = False
    main_memory[open_page].reference_bit = False
    main_memory[open_page].age = age
    global disk_reference, pageFaults
    # When adding a page from memory, it's referencing the disk
    disk_reference += 1
    # A page fault is when memory is on disk rather than RAM
    pageFaults += 1
    return


if __name__ == '__main__':
    program, filename, algo = sys.argv
    algo = algo.upper()
    processes = []
    mainMemory = [MainMemPage() for i in range(32)]
    with open(filename, 'r') as f_input:
        f_input_split = f_input.readlines()
        for age in range(len(f_input_split)):
            line = f_input_split[age]
            # Parse out each line
            processNum, memLocation, action = [int(value) if value.isnumeric() else value for value in line.split()]
            memAction = MemoryAction(processNum, memLocation >> 9, memLocation & 0b1111111, action)
            # Create if process does not exist in system yet
            if processNum > len(processes): processes.append(Process())
            pageLocation = sys.maxsize
            # Check if memory location is in memory or paged.
            for index, page in enumerate(mainMemory):
                if page.process == memAction.processNumber and page.page == memAction.page: pageLocation = index
            if pageLocation == sys.maxsize:
                # Check if there's room in the main memory
                # Remove some page to make room for another page
                if sys.maxsize not in [x.process for x in mainMemory]:
                    # Set page swap algo here
                    if algo == 'RAND': pageLocation = replace_random()
                    if algo == 'FIFO': pageLocation = replace_fifo(mainMemory)
                    if algo == 'LRU': pageLocation = replace_lru(mainMemory)
                    if algo == 'PER': pageLocation = replace_per(mainMemory)
                    if algo == 'BEL': pageLocation = replace_bel(mainMemory, age, f_input_split)
                    print(pageLocation, end=' ')
                    # Depending on algo used, we remove the determined page from memory
                    remove_page(mainMemory, processes, pageLocation)
                    # In the newly freed space, put the new page.
                    add_page(mainMemory, processes, pageLocation, memAction, age)
                # Pages in memory free, just add a new page.
                else:
                    pageLocation = [x.process for x in mainMemory].index(sys.maxsize)
                    add_page(mainMemory, processes, pageLocation, memAction, age)
            if memAction.action == 'W': mainMemory[pageLocation].dirty_bit = True
            if algo == 'LRU': mainMemory[pageLocation].age = age
            if algo == 'PER':
                if age % 200 == 0:
                    for mem in mainMemory:
                        mem.reference_bit = False
                mainMemory[pageLocation].reference_bit = True
    print(filename, algo)
    print("Page Faults:\t\t", pageFaults)
    print("Disk References:\t", disk_reference)
    print("Dirty Writes:\t\t", dirty_writes)

