#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "can_rules_definitions.h"

#define HEADER "#ifndef CAN_RULES_H\n#define CAN_RULES_H\n\n#include \"cansec_rules.h\"\n\nCANRule ruleTable[] = {\n"
#define FOOTNOTE "};\n\nint ruleCount = %d;\n\n#endif // CAN_RULES_H\n"

// Helper functions to map strings to enum values
enum Action parse_action(const char *str)
{
    if (strcmp(str, "alert") == 0)
        return ALERT;
    if (strcmp(str, "block") == 0)
        return BLOCK;
    if (strcmp(str, "drop") == 0)
        return DROP;
    if (strcmp(str, "log") == 0)
        return LOG;
    if (strcmp(str, "pass") == 0)
        return PASS;
    if (strcmp(str, "rewrite") == 0)
        return REWRITE;
    fprintf(stderr, "Unknown action: %s\n", str);
    exit(EXIT_FAILURE);
}

enum Direction parse_direction(const char *str)
{
    if (strcmp(str, "->") == 0)
        return TRANSMIT;
    if (strcmp(str, "<-") == 0)
        return RECEIVE;
    if (strcmp(str, "<>") == 0)
        return BIDIRECTIONAL;
    fprintf(stderr, "Unknown direction: %s\n", str);
    exit(EXIT_FAILURE);
}

// Helper functions to map enum values to strings
const char *action_to_string(enum Action action)
{
    switch (action)
    {
    case ALERT:
        return "ALERT";
    case BLOCK:
        return "BLOCK";
    case DROP:
        return "DROP";
    case LOG:
        return "LOG";
    case PASS:
        return "PASS";
    case REWRITE:
        return "REWRITE";
    default:
        return "UNKNOWN_ACTION";
    }
}

const char *direction_to_string(enum Direction direction)
{
    switch (direction)
    {
    case RECEIVE:
        return "RECEIVE";
    case TRANSMIT:
        return "TRANSMIT";
    case BIDIRECTIONAL:
        return "BIDIRECTIONAL";
    default:
        return "UNKNOWN_DIRECTION";
    }
}

enum OptionType parse_optiontype(const char *str)
{
    if (strcmp(str, "UpLimit") == 0)
        return UpLimit;
    if (strcmp(str, "DownLimit") == 0)
        return DownLimit;
    if (strcmp(str, "Format") == 0)
        return Format;
    if (strcmp(str, "Length") == 0)
        return Length;
    if (strcmp(str, "Message") == 0)
        return Message;
    if (strcmp(str, "Contains") == 0)
        return Contains;
    fprintf(stderr, "Unknown optiontype: %s\n", str);
    exit(EXIT_FAILURE);
}


const char *optiontype_to_string(enum OptionType option)
{
    switch (option)
    {
    case UpLimit:
        return "UpLimit";
    case DownLimit:
        return "DownLimit";
    case Format:
        return "Format";
    case Length:
        return "Length";
    case Message:
        return "Message";
    case Contains:
        return "Contains";
    default:
        return "UNKNOWN_OPTION";
    }
}

// Helper function to parse options
CANSecOption parse_option(char *option_line)
{
    CANSecOption option;
    char *type = strtok(option_line, ":");
    while (*type == ' ')
        type++;
    option.type = parse_optiontype(type);
    printf("Type :%s\n", type);

    char *value = strtok(NULL, ";");
    printf("Value :%s\n", value);
    option.value = (void *)value;
    return option;
}

CANRule init_can_rule(const char *action_str, const char *extended_str, const char *id_str, const char *isRequest_str,
                      const char *direction_str) {
    CANRule rule;
    rule.action = parse_action(action_str);
    rule.extended = (extended_str[0] == 'E');
    rule.id = strtoul(id_str, NULL, 16);
    rule.isRequest = (isRequest_str[0] == 'R');
    rule.dir = parse_direction(direction_str);
    rule.options = NULL;
    return rule;
}

char* get_option_value_pattern(enum OptionType type)
{
    switch (type)
    {
        case UpLimit:
        case DownLimit:
            // TODO : This regex with extended support almost works
            //  (Removing anything before the pipe does not result in any match fail)
            return "^[[:digit:]]-[[:digit:]]\\|[[:digit:]]$\0";
        case Format:
            return "^[[:xdigit:]]\\{1,\\}$\0";
        case Length:
            return "^[[:digit:]]$\0";
        case Message:
            return "^\".*\"$\0";
        case Contains:
            return "^[01]\\{1,\\}$\0";
    }
}

bool validate_option(CANSecOption option)
{
    regex_t regex;
    int reti;
    char* pattern = get_option_value_pattern(option.type);

    /* Compile regular expression */
    reti = regcomp(&regex, pattern, 0);
    if(reti)
    {
        fprintf(stderr, "Could not compile regex\n");
        return false;
    }

    /* Execute regular expression */
    int flag = 0;
    if (option.type == UpLimit || option.type == DownLimit)
    {
        flag = REG_EXTENDED;
    }
    reti = regexec(&regex, option.value, 0, NULL, flag);

    if (reti > REG_NOMATCH)
    {
        char msgbuf[100];
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return false;
    }

    /* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex);
    return reti == REG_NOERROR;
}

// Main function to read the can_rules.txt and generate can_rules.h
int main()
{
    FILE *input = fopen("can_rules.txt", "r");
    if (!input)
    {
        perror("Failed to open can_rules.txt");
        return EXIT_FAILURE;
    }

    FILE *output = fopen("can_rules.h", "w");
    if (!output)
    {
        perror("Failed to open can_rules.h");
        fclose(input);
        return EXIT_FAILURE;
    }

    fprintf(output, HEADER);

    char line[1024], rule[1024];

    int rule_count=0;

    while (fgets(line, sizeof(line), input))
    {
        // Initial parsing for the main components
        char action_str[16], extended_str[2], id_str[16], isRequest_str[2], direction_str[3];
        int num_scanned = sscanf(line, "%15s %1s %15s %1s %2s",
                                 action_str, extended_str, id_str, isRequest_str, direction_str);

        if (num_scanned != 5)
        {
            fprintf(stderr, "Invalid line format: %s", line);
            fclose(input);
            fclose(output);
            return EXIT_FAILURE;
        }

        rule_count++;
        // Parsing main components
        CANRule current_rule = init_can_rule(action_str, extended_str, id_str, isRequest_str, direction_str);

        snprintf(rule, 255, "    { %s, %s, %lu, %s, %s, ",
                 action_to_string(current_rule.action), current_rule.extended ? "true" : "false", current_rule.id,
                 current_rule.isRequest ? "true" : "false", direction_to_string(current_rule.dir));

        // Get the next line for options
        if (!fgets(line, sizeof(line), input))
            continue;

        char *start_options = strchr(line, '(');
        if (!start_options) {
            snprintf(rule, 8, "NULL},\n");
            fprintf(stderr, "No options in rule: %s", line);
            fclose(input);
            fclose(output);
            return EXIT_FAILURE;
        }

        start_options++; // Move past the '('
        strcat(rule, "{");
        //CANSecOption options[10];
        int num_options = 0;
        while (fgets(line, sizeof(line), input))
        {
            if (strchr(line, ')'))
                break;
            num_options++;
            CANSecOption option = parse_option(line);
            // Here you can store the option in a dynamic array if needed
            // For now, just print the parsed options
            if (!validate_option(option))
            {
                printf("The option is not right!\nExpected pattern: %s, got \"%s\"\n",
                       get_option_value_pattern(option.type), (char *) option.value);
                //return EXIT_FAILURE;
            }else { printf("The option is well formatted!\n");}
            char optionString[1024];
            char* format = "{%s,\"%s\"},";
            if (option.type == Message)
                format = "{%s,%s},";
            snprintf(optionString, 128, format, optiontype_to_string(option.type), (char *)option.value);
            strcat(rule, optionString);
        }

        rule[strlen(rule) - 1] = '}';

        char num_option_str[10];
        snprintf(num_option_str, 10, ",%d},\n", num_options);
        strcat(rule, num_option_str);

        // Writing the rule to the output file
        fprintf(output, "%s", rule);
    }

    fprintf(output, FOOTNOTE, rule_count);

    fclose(input);
    fclose(output);

    return EXIT_SUCCESS;
}
