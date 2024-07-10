# HIVIDS_RuleParser
This project is meant to be used with the HIVIDS project, see github.com/BastienMtn/HIVIDS .
This program generates a **can_rules.h** C header file, that can be included in the HIVIDS project, from a **can_rules.txt** file.

## Fonctionnality
The HIVIDS project uses a particular rule format, that must be included in a secured and embedded environment. Therefore, this program needs to store the rules for the IDS inside the binary code itself, and for this reason the rule table must be included as a variable of the project.
Since the representation used by HIVIDS is not user friendly, we propose this solution to simplify the addition of personnalized rules. Users can create their own rule file in a humanly understandable format **(see Input section)**, and this program parses it into a computer understandable format, to be used with the IDS.

## How to use
Download the code from this repository.
Unzip in your desired folder.
Compile the code with :
'''gcc generate_can_rules_header.c -o generate_can_rules_header'''

Include your "can_rules.txt" file in the same folder as the compiled program.

Launch the program with : '''./generate_can_rules_header'''

## Input
The input must be a single text file named 'can_rules.txt' included in the same directory as the parser.
The file must respect the CAN Rule file structure, which is further detailed in the [rule sybtax guide](IDSCAN_RulesSyntax.md).

## Output
The output of this program is a 'can_rules.h' file, written in the same directory as the parser program.
This file contains a rule table formatted in the right format for HIVIDS, which you can then paste in your HIVIDS directory, before compiling and programming your hardware device.
This file structure is not detailed here as it is not necessary to use this program, for further details please referr to the [HIVIDS project repository](https://github.com/BastienMtn/HIVIDS).
