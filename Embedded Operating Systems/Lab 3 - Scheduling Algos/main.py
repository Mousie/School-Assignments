import sys
from collections import namedtuple

process = namedtuple("process", "name deadline wcet")
completedJob = namedtuple("completedJob", "timeStarted name frequency duration energy deadline timeEarlyBy")


def schedule(processor: dict, instance_queu: list[dict], scheduleType, EE):
    run_queue = list()
    schedule = list()
    failure = False

    time = 1
    time_idle_total = 0
    time_exec_total = 1
    level = 0
    # While there's still jobs left to be added
    while len(instance_queue) > 1:
        # Add jobs from the front of the queue to the run list.
        while instance_queue[0]["start"] <= time:
            # Remove them from the queue where we pre-determined order they appear
            run_queue.append(instance_queue.pop(0))
            # Set frequency and duration, level defaults to 0 for full speed scheduling.
            if EE:
                if len(schedule) > 0:
                    earlies = [x.timeEarlyBy for x in schedule if x.timeEarlyBy > 0]
                    avg = sum(earlies) / len(earlies) / max(earlies)  # Take the average of values, normalize by largest
                    if avg < 0.58: level = 0    # Adjust frequencies that new jobs run at based on previous jobs
                    if 0.58 < avg and avg < 0.75: level = 1     # remaining time til their deadline.
                    if 0.75 < avg and avg < 0.85: level = 2
                    if 0.85 < avg: level = 3
            run_queue[-1]["frequency"] = run_queue[-1]["wcet"][level][0]
            run_queue[-1]["duration"] = run_queue[-1]["wcet"][level][1]
            # Underflow protection
            if len(instance_queue) == 0: break
        if scheduleType.lower() == 'edf': run_queue.sort(key=lambda x: (x["deadline"], x["duration"]))
        elif scheduleType.lower() == "rm": run_queue.sort(key=lambda x: (x["priority"], x["duration"]))
        else: print("Wrong schedule type"); return
        # Underflow protection
        if len(run_queue) > 0:
            # Underflow protection and checking if we need to pause to add things to the queue
            while len(run_queue) > 0 and time < instance_queue[0]["start"]:
                if (run_queue[0]["duration"] + time) <= instance_queue[0]["start"]:
                    run_time = run_queue[0]["duration"]
                    schedule.append(completedJob(time, 'w' + str(run_queue[0]["name"]), run_queue[0]["frequency"],
                            run_time, processor[str(run_queue[0]["frequency"])] * run_time,run_queue[0]["deadline"],
                            run_queue[0]["deadline"] - time - run_time))
                    time_exec_total += run_time
                    time += run_time
                    run_queue.pop(0)
                else:
                    run_time = instance_queue[0]["start"] - time
                    run_queue[0]["duration"] -= run_time
                    schedule.append(completedJob(time, 'w' + str(run_queue[0]["name"]), run_queue[0]["frequency"],
                            run_time, processor[str(run_queue[0]["frequency"])] * run_time, run_queue[0]["deadline"],
                            run_queue[0]["deadline"] - time - run_time))
                    time_exec_total += run_time
                    time += run_time
                if schedule[-1].deadline < time:
                    run_queue.clear()
                    instance_queue.clear()
                    failure = True
        else:
            time_idle = instance_queue[0]["start"] - time if len(instance_queue) > 0 else processor["sysExeTime"] - time
            schedule.append(completedJob(time, 'Idle', 'None', time_idle, processor["idle"] * time_idle, 0, 0))
            time += time_idle
            time_idle_total += time_idle
    if processor["sysExeTime"] - time > 0 and not failure:
        schedule.append(completedJob(time, 'Idle', 'None', processor["sysExeTime"] - time, processor["idle"] * (processor["sysExeTime"] - time), 0, 0))
        time_idle_total += processor["sysExeTime"] - time
    totalJoules = 0
    for i in schedule:
        energy = i.energy * 0.001
        print(f"{str(i.timeStarted).rjust(4)}\t{i.name.rjust(4)}\t{str(i.frequency).rjust(4)}"
              f"\t{str(i.duration).rjust(3)}\t{((str(round(energy, 3))) + ' J').rjust(9)}\t{i.timeEarlyBy}")
        totalJoules += energy
    print(f"Total Energy: {round(totalJoules, 4)} J\n"
          f"Total Execution Time: {time_exec_total}\n"
          f"Total Idle Time: {time_idle_total}\n"
          f"Percent Idle: {round(100 * time_idle_total / (time_idle_total + time_exec_total), 2)}%")
    if failure == True: print("Deadline Failure")
    return

if __name__ == '__main__':
    processes = list()
    with open(sys.argv[1], 'r') as raw_f_input:
        f_input = raw_f_input.read().split('\n')
        processorVars = ["numOfProcesses", "sysExeTime", "1188", "918", "648", "384", "idle"]
        processor = dict(zip(processorVars, [int(x) for x in f_input[0].split()]))
        for line in f_input[1:-1]:
            temp = [int(x) for x in line[1:].split()]
            processes.append(process(temp[0], temp[1], ((1188, temp[2]), (918, temp[3]), (648, temp[4]), (384, temp[5]))))
    #schedulability = processor.numOfProcesses*(2**(1/processor.numOfProcesses)-1)
    if sys.argv[2] == "RM": print("RM: Rate Monotonic")
    elif sys.argv[2] == "EDF": print("EDF: Earliest Deadline First")
    # Create set of jobs to put into processor queue
    instance_queue = list()
    # Create set of jobs to put into processor queue
    processes.sort(key=lambda x: x.deadline)
    for priority, process in enumerate(processes):
        instance_queue.extend([{"name": process.name,"start": n * process.deadline,
                                "deadline": (n + 1) * process.deadline, "priority": priority,"wcet": process.wcet}
            for n in range(((processor["sysExeTime"] - 1) // process.deadline) + 1)])
    instance_queue.sort(key=lambda process: process["start"])
    instance_queue.append({"start": sys.maxsize})  # Add dummy job to act as end of queue sentinel.
    if len(sys.argv) > 3:
        if sys.argv[3] == "EE": schedule(processor, instance_queue, sys.argv[2], True)
    else: schedule(processor, instance_queue, sys.argv[2], False)

