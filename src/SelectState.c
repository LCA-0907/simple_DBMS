#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Command.h"
#include "SelectState.h"

void field_state_handler(Command_t *cmd, size_t arg_idx) {
    cmd->cmd_args.sel_args.fields = NULL;
    cmd->cmd_args.sel_args.fields_len = 0;
    cmd->cmd_args.sel_args.limit = -1;
    cmd->cmd_args.sel_args.offset = -1;
    
    cmd->cmd_args.sel_args.where = -1;
    cmd->cmd_args.sel_args.field1 = -1;
    cmd->cmd_args.sel_args.field2 = -1;
    cmd->cmd_args.sel_args.operator1 = -1;
    cmd->cmd_args.sel_args.operator2 = -1;
    cmd->cmd_args.sel_args.num1 = -1;
    cmd->cmd_args.sel_args.num2 = -1;
    while(arg_idx < cmd->args_len) {
        if (!strncmp(cmd->args[arg_idx], "*", 1)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "id", 2)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "name", 4)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "email", 5)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "age", 3)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "from", 4)) {
            table_state_handler(cmd, arg_idx+1);
            return;
        } else {
            cmd->type = UNRECOG_CMD;
            return;
        }
        arg_idx += 1;
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void table_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len
            && !strncmp(cmd->args[arg_idx], "table", 5)) {
        arg_idx++;
        if (arg_idx == cmd->args_len) {
            return;
        } else if(!strncmp(cmd->args[arg_idx], "where", 5)){
        	//perror("where");
        	where_state_handler(cmd, arg_idx+1);
        	//perror("where end");
        	return;	
        } else if (!strncmp(cmd->args[arg_idx], "offset", 6)) {
            offset_state_handler(cmd, arg_idx+1);
            return;
        } else if (!strncmp(cmd->args[arg_idx], "limit", 5)) {
            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

//retune char num of operator

int check_operator(Command_t *cmd, char* operate, int operator_num){
	int *op;
	if(operator_num==1)
		op = &(cmd->cmd_args.sel_args.operator1);
	else
		op = &(cmd->cmd_args.sel_args.operator2);
		
		
	if(strstr(operate, "="))//= operator
	{	 
		if(strstr(operate, "!="))//!= operator
		{	
			*op = 1;
			return 2;
		}
		else if(strstr(operate, ">="))//>=
	    {	*op = 4; 
		    return 2;
	    }
	    else if(strstr(operate, "<="))//<=
		{	*op = 5;
			return 2;
		}
		else
		{
			*op = 0;
			return 1;
		}
	}
	
	else if(strstr(operate, ">"))//>
	{
		if(strstr(operate, ">="))//>=
		{	*op = 4; 
			return 2;
		}
		else
		{	*op = 2;
			return 1;
		}
	}
	else if(strstr(operate, "<"))//<
	{
		if(strstr(operate, "<="))//<=
		{	*op = 5;
			return 2;
		}
		else
		{	*op = 3; 
			return 1;
		}
	}
	return 0;
}

void where_state_handler(Command_t *cmd, size_t arg_idx) {
    cmd->cmd_args.sel_args.where++;
    if (arg_idx < cmd->args_len) 
    {
    
        if (!strncmp(cmd->args[arg_idx], "id", 2)) 
        {
        	perror("id");
            cmd->cmd_args.sel_args.field1 = 0;
            if(strlen(cmd->args[arg_idx])>2)// id> x or id>x
            {
            	int c_idx = 2 + check_operator(cmd, cmd->args[arg_idx], 1);
            	if(atoi(cmd->args[arg_idx]+c_idx))// id>x
            	{
            		cmd->cmd_args.sel_args.num1 = atoi(cmd->args[arg_idx]+c_idx);
            	}
            	else if(arg_idx < cmd->args_len) //id> x
            	{
            		arg_idx++;
            		cmd->cmd_args.sel_args.num1 = atoi(cmd->args[arg_idx]);
            	}
            	
            }
            else if(arg_idx < cmd->args_len) // id >x or id > x check opeartor
            {
        		arg_idx++; //>x or >
        		int c_idx =check_operator(cmd, cmd->args[arg_idx], 1);
        		if(atoi(cmd->args[arg_idx] + c_idx)) //>x
        		{
        			cmd->cmd_args.sel_args.num1 = atoi(cmd->args[arg_idx]+c_idx);
        		}
        		else if(arg_idx < cmd->args_len) // > x check num
            	{	
        			arg_idx++;
        			cmd->cmd_args.sel_args.num1 = atoi(cmd->args[arg_idx]);        			
        		}
        	}
        	
        } 
        else if (!strncmp(cmd->args[arg_idx], "name", 4)) 
        {
            perror("name\n");
            cmd->cmd_args.sel_args.field1 = 1;
            if(arg_idx < cmd->args_len) // check opeartor
            {
        		arg_idx++;
        		check_operator(cmd, cmd->args[arg_idx], 1);
				
        	}
        	if(arg_idx < cmd->args_len) // cpy str
            {
        		arg_idx++;
        		strcpy(cmd->cmd_args.sel_args.str1,cmd->args[arg_idx]);
        	}
            /*if(strlen(cmd->args[arg_idx])>2)// id>x
            {
            	Spliter(cmd->args[arg_idx], 4, cmd);
            }*/
        } 
        else if (!strncmp(cmd->args[arg_idx], "email", 5)) 
        {
            perror("email\n");
            cmd->cmd_args.sel_args.field1 = 2;
            if(arg_idx < cmd->args_len) // check opeartor
            {
            	arg_idx++;
        		check_operator(cmd, cmd->args[arg_idx], 1);
        	}
        	if(arg_idx < cmd->args_len) // cpy str
            {
        		arg_idx++;
        		strcpy(cmd->cmd_args.sel_args.str1,cmd->args[arg_idx]);
        	}
            /*if(strlen(cmd->args[arg_idx])>2)// id>x
            {
            	Spliter(cmd->args[arg_idx], 5, cmd);
            }*/
        } 
        else if (!strncmp(cmd->args[arg_idx], "age", 3)) 
        {
            perror("age\n");
            cmd->cmd_args.sel_args.field1 = 3;
            if(strlen(cmd->args[arg_idx])>3)// age> x or id>x
            {
            	int c_idx = 3 + check_operator(cmd, cmd->args[arg_idx], 1);
            	if(atoi(cmd->args[arg_idx]+c_idx))// id>x
            	{
            		cmd->cmd_args.sel_args.num1 = atoi(cmd->args[arg_idx]+c_idx);
            	}
            	else if(arg_idx < cmd->args_len) //id> x
            	{
            		arg_idx++;
            		cmd->cmd_args.sel_args.num1 = atoi(cmd->args[arg_idx]);
            	}
            	
            }
            else if(arg_idx < cmd->args_len) // id >x or id > x check opeartor
            {
        		arg_idx++; //>x or >
        		int c_idx =check_operator(cmd, cmd->args[arg_idx], 1);
        		if(atoi(cmd->args[arg_idx] + c_idx)) //>x
        		{
        			cmd->cmd_args.sel_args.num1 = atoi(cmd->args[arg_idx]+c_idx);
        		}
        		else if(arg_idx < cmd->args_len) // > x check num
            	{	
        			arg_idx++;
        			cmd->cmd_args.sel_args.num1 = atoi(cmd->args[arg_idx]);        			
        		}
        	}
           	
        }     
 
    }
  	arg_idx++;
		
        if (arg_idx == cmd->args_len) {
            return;
        } 
        
        else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "offset", 6)){
            perror("in to offset");
            offset_state_handler(cmd, arg_idx+1); 
            return;   
        }
        else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "limit", 5)) {
			perror("in to limit");
            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    
    cmd->type = UNRECOG_CMD;
    return;
}


void offset_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.offset = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        } else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "limit", 5)) {

            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void limit_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.limit = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}
