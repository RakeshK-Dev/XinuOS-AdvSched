# Xinu Advanced Scheduling

## Project Description
This repository contains the implementation of process management and scheduling algorithms in the Xinu operating system as part of the ECE465/565 Operating Systems Design course. The project aims to enhance understanding of process creation, context switching, and scheduling by implementing two key scheduling policies: **Lottery Scheduling** and **Multi-Level Feedback Queue (MLFQ)**.

## Overview
### Key Learning Objectives:
- Deep dive into Xinu's process management implementation.
- Implement and validate advanced scheduling algorithms.
- Understand and handle context switches and process prioritization.

### Project Parts:
1. **Part 1: Understanding and Extending Xinu's Process Management**
   - Analyzed Xinu's source code related to process creation, scheduling, and context switching.
   - Added utility functions like `print_ready_list()` to display the ready processes.
   - Enhanced the Process Control Block (PCB) with runtime tracking fields and created a `create_user_process()` function to manage user processes.

2. **Part 2: Lottery Scheduling**
   - Implemented `set_tickets()` to assign scheduling tickets for user processes.
   - Modified the scheduler to conduct a lottery among user processes, maintaining system process priority.
   - Conducted fairness analysis through test cases and runtime data plots.

3. **Part 3: MLFQ Scheduling (Graduate-Level)**
   - Developed a Multi-Level Feedback Queue scheduling policy with configurable priority queues and dynamic time allotments.
   - Implemented priority boosting and tracked process priority changes.
   - Validated the implementation using provided and custom test cases.

## Implementation Challenges
### Encountered Coding Problems:
- **Maintaining Code Integrity**: Ensuring a clean and original codebase across different parts while adhering to academic integrity guidelines.
- **Debugging Complex Scheduling**: Instrumenting `resched.c` to handle context switch logs and ensuring correct scheduling decisions for user and system processes.
- **Testing and Validation**: Testing on the course's VCL image to confirm compatibility and behavior. Handling variations in runtime output during test cases.
- **Fairness Analysis**: Collecting and analyzing data to ensure fair CPU time distribution, especially in Lottery Scheduling.

## Lessons Learned:
- Enhanced understanding of process scheduling and context switching mechanics in an operating system.
- Gained hands-on experience in implementing and debugging complex scheduling algorithms.
- Learned to manage project structure and maintain modular code for extensibility and testing.

## Usage:
To test or run this project:
1. Copy the `xinu-vbox` folder and follow the submission guidelines to set up the Xinu environment.
2. Modify the necessary files as described in the project parts.
3. Run test cases using provided or custom `main.c` files to validate scheduling behavior.

## Conclusion:
This project showcases the understanding and application of process management and scheduling in an OS. It demonstrates the implementation of algorithms that balance CPU time distribution and prioritize tasks effectively. The repository also serves as a learning resource for peers interested in operating systems and scheduling policies.

