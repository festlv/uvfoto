#include <stdio.h>
#include "gcode.h"
#include "stepper.h"
#include "stellariscommon.h"


static char command_buffer[500];

static int16_t buffer_counter=0;

static void gcode_execute_command(char cmd, int command_num, char * command) {
    float tmppos;

    if (cmd=='G') {
        switch (command_num) {
            case 28:
                stepper_move_to_origin();
                break;
            case 1:
                if (sscanf(command, "G1 X%f", &tmppos) == 1)
                    stepper_move_position_blocking_def(tmppos);

                break;
            case 92:
                if (sscanf(command, "G92 X%f", &tmppos)==1)
                    stepper_set_position(tmppos);
                else
                    stepper_set_position(0.0);
                break;
            default:
                printf("Unsupported command!\n");
                return;

    }
    }

    printf("OK\n");
}


static void gcode_parse_command() {
    int command_num;
    char command;
    int res = sscanf(command_buffer, "%c%d", &command, &command_num);
    if (res==2) {
        gcode_execute_command(command, command_num, command_buffer);

    } 
}

void gcode_step() {
    char tmp;
    static uint8_t ignore_rest_input = 0;

    if (CharsAvail()) {
        tmp = ReadChar();
        if (tmp==';') {
            ignore_rest_input = 1;
            command_buffer[buffer_counter]='\0';
        }

        if (!ignore_rest_input) {
            command_buffer[buffer_counter] = tmp;
            buffer_counter++;
        }        
        if (tmp=='\n') {
            if (!ignore_rest_input)
                command_buffer[buffer_counter] = '\0';
            gcode_parse_command();
            buffer_counter = 0; 
        }
    }
}


