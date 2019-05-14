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
    cmd->cmd_args.sel_args.andor = 0;
    
   	cmd->cmd_args.sel_args.agg = 0;
    cmd->cmd_args.sel_args.sum = 0;
    
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
        } else if (!strncmp(cmd->args[arg_idx], "sum", 3)){
        	add_select_field(cmd, cmd->args[arg_idx]);
        	cmd->cmd_args.sel_args.agg = 1;
        } else if (!strncmp(cmd->args[arg_idx], "avg", 2)){
        	add_select_field(cmd, cmd->args[arg_idx]);
        	cmd->cmd_args.sel_args.agg = 1;
        } else if (!strncmp(cmd->args[arg_idx], "count", 5)){
        	add_select_field(cmd, cmd->args[arg_idx]);
        	cmd->cmd_args.sel_args.agg = 1;
        } else if (!strncmp(cmd->args[arg_idx], "from", 4)){
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
    cmd->cmd_args.sel_args.where = cmd->cmd_args.sel_args.where+1;
   // printf("where count:%d\n",cmd->cmd_args.sel_args.where);
    int *field;
    char *str;
    double *num;
    int op_num;
    if(cmd->cmd_args.sel_args.where==0){//first into where
    	field = &cmd->cmd_args.sel_args.field1;
    	str = cmd->cmd_args.sel_args.str1;
    	num = &cmd->cmd_args.sel_args.num1;
    	op_num = 1;
    }
    else if(cmd->cmd_args.sel_args.where==1){
    	field = &cmd->cmd_args.sel_args.field2;
    	str = cmd->cmd_args.sel_args.str2;
    	num = &cmd->cmd_args.sel_args.num2;
    	op_num = 2;
    }
    if (arg_idx < cmd->args_len) 
    {
        if (!strncmp(cmd->args[arg_idx], "id", 2)) 
        {
        	//perror("id");
            *field = 0;
            if(strlen(cmd->args[arg_idx])>2)// id> x or id>x
            {
            	int c_idx = 2 + check_operator(cmd, cmd->args[arg_idx], op_num);
            	if(atoi(cmd->args[arg_idx]+c_idx))// id>x
            	{
            		*num = atoi(cmd->args[arg_idx]+c_idx);
            	}
            	else if(arg_idx < cmd->args_len) //id> x
            	{
            		arg_idx++;
            		*num = atoi(cmd->args[arg_idx]);
            	}
            	
            }
            else if(arg_idx < cmd->args_len) // id >x or id > x check opeartor
            {
        		arg_idx++; //>x or >
        		int c_idx =check_operator(cmd, cmd->args[arg_idx], op_num);
        		if(atoi(cmd->args[arg_idx] + c_idx)) //>x
        		{
        			*num = atoi(cmd->args[arg_idx]+c_idx);
        		}
        		else if(arg_idx < cmd->args_len) // > x check num
            	{	
        			arg_idx++;
        			*num = atoi(cmd->args[arg_idx]);        			
        		}
        	}
        	
        } 
        else if (!strncmp(cmd->args[arg_idx], "name", 4)) 
        {
            //perror("name\n");
            *field = 1;
            if(strlen(cmd->args[arg_idx])>4)// name= x or name= x
            {
            	int c_idx = 4 + check_operator(cmd, cmd->args[arg_idx], op_num);
            	if(cmd->args[arg_idx][c_idx]!='\0')// name=x
            	{
            		strcpy(str, cmd->args[arg_idx] + c_idx);
            	}
            	else if(arg_idx < cmd->args_len) //name= x
            	{
            		arg_idx++;
            		strcpy(str, cmd->args[arg_idx]);
            	}
            	
            }
            else if(arg_idx < cmd->args_len) // name =x or name = x check opeartor
            {
        		arg_idx++; //=x or = x
        		int c_idx =check_operator(cmd, cmd->args[arg_idx], op_num);
        		if(cmd->args[arg_idx][c_idx]!='\0') //=x
        		{
        			strcpy(str, cmd->args[arg_idx] + c_idx);
        		}
        		else if(arg_idx < cmd->args_len) // = x check num
            	{	
        			arg_idx++;
        			strcpy(str, cmd->args[arg_idx]);       			
        		}
        	}
        } 
        else if (!strncmp(cmd->args[arg_idx], "email", 5)) 
        {
            //perror("email\n");
            *field = 2;
            if(strlen(cmd->args[arg_idx])>5)// email= x or email= x
            {
            	int c_idx = 5 + check_operator(cmd, cmd->args[arg_idx], op_num);
            	if(cmd->args[arg_idx][c_idx]!='\0')// email=x
            	{
            		strcpy(str, cmd->args[arg_idx] + c_idx);
            	}
            	else if(arg_idx < cmd->args_len) //email= x
            	{
            		arg_idx++;
            		strcpy(str ,cmd->args[arg_idx]);
            	}
            	
            }
            else if(arg_idx < cmd->args_len) // name =x or name = x check opeartor
            {
        		arg_idx++; //=x or = x
        		int c_idx =check_operator(cmd, cmd->args[arg_idx], op_num);
        		if(cmd->args[arg_idx][c_idx]!='\0') //=x
        		{
        			strcpy(str, cmd->args[arg_idx] + c_idx);
        		}
        		else if(arg_idx < cmd->args_len) // = x check num
            	{	
        			arg_idx++;
        			strcpy(str, cmd->args[arg_idx]);       			
        		}
        	}
            
        } 
        else if (!strncmp(cmd->args[arg_idx], "age", 3)) 
        {
            //perror("age\n");
            *field = 3;
            if(strlen(cmd->args[arg_idx])>3)// age> x or age>x
            {
            	int c_idx = 3 + check_operator(cmd, cmd->args[arg_idx], op_num);
            	if(atoi(cmd->args[arg_idx]+c_idx))// 
            	{
            		*num = atoi(cmd->args[arg_idx]+c_idx);
            	}
            	else if(arg_idx < cmd->args_len) 
            	{
            		arg_idx++;
            		*num = atoi(cmd->args[arg_idx]);
            	}
            	
            }
            else if(arg_idx < cmd->args_len) //  check opeartor
            {
        		arg_idx++; //>x or >
        		int c_idx =check_operator(cmd, cmd->args[arg_idx], op_num);
        		if(atoi(cmd->args[arg_idx] + c_idx)) //>x
        		{
        			*num = atoi(cmd->args[arg_idx]+c_idx);
        		}
        		else if(arg_idx < cmd->args_len) // > x check num
            	{	
        			arg_idx++;
        			*num = atoi(cmd->args[arg_idx]);        			
        		}
        	}
           	
        }     
 
    }
  	arg_idx++;
		
        if (arg_idx == cmd->args_len) {
            return;
        } 
        else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "and", 3)){
                cmd->cmd_args.sel_args.andor = 1;
               // perror("and");
                where_state_handler(cmd, arg_idx+1);
        }
        else if(arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "or", 2)){
                cmd->cmd_args.sel_args.andor = 2;
                where_state_handler(cmd, arg_idx+1);
        }
        else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "offset", 6)){
           // perror("in to offset");
            offset_state_handler(cmd, arg_idx+1); 
            return;   
        }
        else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "limit", 5)) {
			//perror("in to limit");
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
