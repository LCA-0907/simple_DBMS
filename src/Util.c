#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "SelectState.h"
#define TRUE 1
#define FALSE 0

///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id", 2)) {
                printf("%d", user->id);
            } else if (!strncmp(sel_args->fields[idx], "name", 4)) {
                printf("%s", user->name);
            } else if (!strncmp(sel_args->fields[idx], "email", 5)) {
                printf("%s", user->email);
            } else if (!strncmp(sel_args->fields[idx], "age", 3)) {
                printf("%d", user->age);
            }
        }
    }
    printf(")\n");
}


int where_ok_check(int op, int field1, int num1, char str1[50], User_t *Utemp)
{
	int whereOK=FALSE;
	for(int i=0;i<4;i++)
    {
        if(field1==0)//id
        {
            if(op==0)//=
            {
            	if((double)(Utemp->id)==num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==1)//!=
            {
            	if((double)(Utemp->id)!=num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==2)//>
            {
            	if((double)(Utemp->id)>num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==3)//<
            {
            	if((double)(Utemp->id)<num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==4)//>=
            {
            	if((double)(Utemp->id)>=num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==5)//<=
            {
            	if((double)(Utemp->id)<=num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
        }
        else if(field1==1)//name
        {
            if(op==0)//=
            {
            	if(!strcmp(Utemp->name, str1))
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==1)//!=
            {
            	if(strcmp(Utemp->name, str1))
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
        }
        else if(field1==2)//email
        {
            if(op==0)//=
            {
            	if(!strcmp(Utemp->email, str1))
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==1)//!=
            {
            	if(strcmp(Utemp->email, str1))
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
        }
        else if(field1==3)//age
        {
            if(op==0)//=
            {
            	if((double)(Utemp->age) ==num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==1)//!=
            {
            	if((double)(Utemp->age)!=num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==2)//>
            {
            	if((double)(Utemp->age)>num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==3)//<
            {
            	if((double)Utemp->age<num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==4)//>=
            {
            	if((double)(Utemp->age)>=num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
            else if(op==5)//<=
            {
            	if((double)(Utemp->age)<=num1)
            		whereOK=TRUE;
            	else
            		whereOK=FALSE;
            }
        }
    }
    return whereOK;
}

///
/// calculate agg
///
void cal_agg(Command_t *cmd, User_t *user, SelectArgs_t *sel_args )
{
	size_t idx;
	if(cmd->cmd_args.sel_args.sum_len== 0){
		cmd->cmd_args.sel_args.sum_len = sel_args->fields_len;
		cmd->cmd_args.sel_args.sum = malloc(sizeof(int)*sel_args->fields_len);
	}
		
	for (idx = 0; idx < sel_args->fields_len; idx++) {
		
        if (!strncmp(sel_args->fields[idx], "sum", 3)) {
           if (!strncmp(sel_args->fields[idx]+4, "id", 2)) {
                cmd->cmd_args.sel_args.sum[idx] += user->id;
            } else if (!strncmp(sel_args->fields[idx]+4, "age", 3)) {
                cmd->cmd_args.sel_args.sum[idx] += user->age;
            } 
        } 
        else if (!strncmp(sel_args->fields[idx], "avg", 3)) {
            if (!strncmp(sel_args->fields[idx]+4, "id", 2)) {
                cmd->cmd_args.sel_args.sum[idx] += user->id;
            } else if (!strncmp(sel_args->fields[idx]+4, "age", 3)) {
                cmd->cmd_args.sel_args.sum[idx] += user->age;
            } 
                
    	}
	}
}

///
/// Print the users for given offset and limit restriction
///
void print_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd) {
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;
	int where = cmd->cmd_args.sel_args.where;
	int andor = cmd->cmd_args.sel_args.andor;
	int whereOK = TRUE;
	int whereOK1 = TRUE;
	int whereOK2 = TRUE;
	//printf("where bool:%d\n", cmd->cmd_args.sel_args.where);
	
    if (offset == -1) {
        offset = 0;
    }
    if (where < 0) 
    {
    	//printf("no where\n");
        for (idx = offset; idx < table->len; idx++) {
        	//printf("%ld\n",idx);
            if (limit != -1 && (idx - offset) >= limit) {
                break;
            }
            if(cmd->cmd_args.sel_args.agg){
            		cal_agg(cmd, get_User(table, idx), &(cmd->cmd_args.sel_args));
            }
    	    else{
           		print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));  
           	}
        }
        if(cmd->cmd_args.sel_args.agg && offset == 0 && (limit >= 1 || limit == -1))
        {
        	printf("(");
    		for (idx = 0; idx < cmd->cmd_args.sel_args.fields_len; idx++) {
            	if (idx > 0) printf(", ");
	
            	if (!strncmp(cmd->cmd_args.sel_args.fields[idx], "avg", 3)) {
                	printf("%.3f", (double)cmd->cmd_args.sel_args.sum[idx] / (double)table->len);
            	} else if (!strncmp(cmd->cmd_args.sel_args.fields[idx], "sum", 3)) {
                	printf("%d", cmd->cmd_args.sel_args.sum[idx]);
            	} else if (!strncmp(cmd->cmd_args.sel_args.fields[idx], "count", 5)) {
                	printf("%d", table->len);
            	}
        	}
        	printf(")\n");	
    	}
        
    } else {
    	int printed =0;
        for (idx = 0; idx < table->len; idx++) {
            User_t *Utemp=get_User(table, idx);
            if (limit != -1 && printed-offset >= limit) 
                	break;
            if(where>=0)
            {		
            	//perror("where check");
            	whereOK1 = where_ok_check(cmd->cmd_args.sel_args.operator1, cmd->cmd_args.sel_args.field1, cmd->cmd_args.sel_args.num1, cmd->cmd_args.sel_args.str1, Utemp);
            	whereOK = whereOK1; 
            	if(where==1)
            	{
            		whereOK2 = where_ok_check(cmd->cmd_args.sel_args.operator2, cmd->cmd_args.sel_args.field2, cmd->cmd_args.sel_args.num2, cmd->cmd_args.sel_args.str2, Utemp);
            		if(andor==1)//and
            			whereOK = whereOK1 && whereOK2;
            		else if(andor==2)//or
            			whereOK = whereOK1 || whereOK2;
            		
            	}
            	
            }else
            {
            	if (limit != -1 && (idx - offset) >= limit) {
                	break;
            	}
            }
            if(whereOK){
            //print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));
            	if(cmd->cmd_args.sel_args.agg){
            		cal_agg(cmd, get_User(table, idx), &(cmd->cmd_args.sel_args));
            	}
            	else{
            		if(printed>=offset)
            		{
            			print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));
            		}
            	}
            	printed ++;
            }
        }
        if(cmd->cmd_args.sel_args.agg && offset == 0 && (limit >= 1 || limit == -1))
        {
      	
        	printf("(");
    		for (idx = 0; idx < cmd->cmd_args.sel_args.fields_len; idx++) {
            	if (idx > 0) printf(", ");
	
            	if (!strncmp(cmd->cmd_args.sel_args.fields[idx], "avg", 3)) {
                	printf("%.3f", (double)cmd->cmd_args.sel_args.sum[idx] / (double)printed);
            	} else if (!strncmp(cmd->cmd_args.sel_args.fields[idx], "sum", 3)) {
                	printf("%d", cmd->cmd_args.sel_args.sum[idx]);
            	} else if (!strncmp(cmd->cmd_args.sel_args.fields[idx], "count", 5)) {
                	printf("%d", printed);
            	}
        	}
        	printf(")\n");	
    	}
    }
}

///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " ,\n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        archive_table(table);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    } else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(Table_t *table, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd(table, cmd);
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd(table, cmd);
        return SELECT_CMD;
    } else if (!strncmp(cmd->args[0], "update", 6)){
    	handle_update_cmd(table, cmd);
    	return UPDATE_CMD;
    } else if(!strncmp(cmd->args[0], "delete", 6)){
    	handle_delete_cmd(table, cmd);
    	return DELETE_CMD;
    } else {
        return UNRECOG_CMD;
    }
}



///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;
    User_t *user = command_to_User(cmd);
    if (user) {
        ret = add_User(table, user);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}

///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd, 1);
    print_users(table, NULL, 0, cmd);
    return table->len;
}

///
///
///
int handle_update_cmd(Table_t *table, Command_t *cmd){
	int updated_num=0;
	update_field_handler(cmd, 3);
	size_t idx;
	int OK_num=0;
	size_t *OK_query = malloc(sizeof(size_t));
	for(idx = 0 ; idx<table->len ; idx++)
	{
		int where = cmd->cmd_args.sel_args.where;
        int andor = cmd->cmd_args.sel_args.andor;
        int whereOK = TRUE;
        int whereOK1 = TRUE;
        int whereOK2 = TRUE;
        User_t *Utemp=get_User(table, idx);
        if(where>=0)
        {		
            //perror("where check");
            whereOK1 = where_ok_check(cmd->cmd_args.sel_args.operator1, cmd->cmd_args.sel_args.field1, cmd->cmd_args.sel_args.num1, cmd->cmd_args.sel_args.str1, Utemp);
            whereOK = whereOK1; 
            if(where==1)
            {
            	whereOK2 = where_ok_check(cmd->cmd_args.sel_args.operator2, cmd->cmd_args.sel_args.field2, cmd->cmd_args.sel_args.num2, cmd->cmd_args.sel_args.str2, Utemp);
            	if(andor==1)//and
            		whereOK = whereOK1 && whereOK2;
            	else if(andor==2)//or
            		whereOK = whereOK1 || whereOK2;
            	
            }
            if(whereOK)
            {
            	if(OK_query)
            	{
            		OK_query[OK_num] = idx;
            		OK_num++;
            		OK_query = realloc (OK_query, sizeof(size_t) * OK_num);

            	}
            	
            }	
        }
        else 
        {
        	if(OK_query)
            {
            	OK_query[OK_num] = idx;
            	OK_num++;
            	OK_query = realloc (OK_query, sizeof(size_t) * OK_num);
            	
            }
        	
		}
	}
	if(!strncmp(cmd->cmd_args.sel_args.fields[0], "id", 2)){
		if(OK_num > 1)// no use
			;
		else if(OK_num==1)
		{
			int repeat=FALSE;

			for(idx=0 ; idx < table->len ; idx++)
			{	User_t *Utemp=get_User(table, idx);
				if (Utemp->id == cmd->cmd_args.sel_args.update_num
					&& idx != OK_query[0] )
				{
						repeat=TRUE;
						break;
					
				}
			}
			if(repeat)
				updated_num =0;
			else
			{
				for(idx=0 ; idx < table->len ; idx++)
			    {	User_t *Utemp=get_User(table, idx);
				    if (idx == OK_query[0] )
				    {
    					Utemp->id = cmd->cmd_args.sel_args.update_num;
    					updated_num = 1;
				    }
			    }
			}
		}	
    }
    else if(!strncmp(cmd->cmd_args.sel_args.fields[0], "name", 4)){
    	int ok_idx =0;
    	for(ok_idx=0 ; ok_idx<OK_num ; ok_idx++)
    	{
    		if(ok_idx >= table->len)
    			printf("error idx\n");
    		else
    		{
    			User_t *Utemp = get_User(table, OK_query[ok_idx]);
    			strcpy(Utemp->name, cmd->cmd_args.sel_args.update_str);
    			updated_num++;
    		}
    	}
    }
    else if(!strncmp(cmd->cmd_args.sel_args.fields[0], "email", 5)){
    	int ok_idx =0;
    	for(ok_idx=0 ; ok_idx<OK_num ; ok_idx++)
    	{
    		if(ok_idx >= table->len)
    			printf("error idx\n");
    		else
    		{
    			User_t *Utemp = get_User(table, OK_query[ok_idx]);
    			strcpy (Utemp->email, cmd->cmd_args.sel_args.update_str);
    			updated_num++;
    		}
    	}
    }
    else if(!strncmp(cmd->cmd_args.sel_args.fields[0], "age", 3)){
    	int ok_idx =0;
    	for(ok_idx=0 ; ok_idx<OK_num ; ok_idx++)
    	{
    		if(ok_idx >= table->len)
    			printf("error idx\n");
    		else
    		{
    			User_t *Utemp = get_User(table, OK_query[ok_idx]);
    			Utemp->age = cmd->cmd_args.sel_args.update_num;
    			updated_num++;
    		}
    	}
    }
	
	perror("update");
	return updated_num;
	
}

///
/// handle delete
///
int handle_delete_cmd(Table_t *table, Command_t *cmd){
	int deleted_num=0;
	update_field_handler(cmd, 3);
	size_t idx;
	int OK_num=0;
	size_t *OK_query = malloc(sizeof(size_t));
	for(idx = 0 ; idx<table->len ; idx++)
	{
		int where = cmd->cmd_args.sel_args.where;
        int andor = cmd->cmd_args.sel_args.andor;
        int whereOK = TRUE;
        int whereOK1 = TRUE;
        int whereOK2 = TRUE;
        User_t *Utemp=get_User(table, idx);
        if(where>=0)
        {		
            //perror("where check");
            whereOK1 = where_ok_check(cmd->cmd_args.sel_args.operator1, cmd->cmd_args.sel_args.field1, cmd->cmd_args.sel_args.num1, cmd->cmd_args.sel_args.str1, Utemp);
            whereOK = whereOK1; 
            if(where==1)
            {
            	whereOK2 = where_ok_check(cmd->cmd_args.sel_args.operator2, cmd->cmd_args.sel_args.field2, cmd->cmd_args.sel_args.num2, cmd->cmd_args.sel_args.str2, Utemp);
            	if(andor==1)//and
            		whereOK = whereOK1 && whereOK2;
            	else if(andor==2)//or
            		whereOK = whereOK1 || whereOK2;
            	
            }
            if(whereOK)
            {
            	if(OK_query)
            	{
            		OK_query[OK_num] = Utemp->id;
            		OK_num++;
            		OK_query = realloc (OK_query, sizeof(size_t) * OK_num);

            	}
            	
            }	
        }
        else 
        {
        	if(OK_query)
            {
            	OK_query[OK_num] = Utemp->id;
            	OK_num++;
            	OK_query = realloc (OK_query, sizeof(size_t) * OK_num);
            	
            }
        	
		}
	}
	int ok_idx;
	for(ok_idx=0 ; ok_idx<OK_num ; ok_idx++)
	{
		for(idx = 0 ; idx<table->len ; idx++)
	    {	
		    User_t *Utemp=get_User(table, idx);
		    if(Utemp->id == OK_query[ok_idx])
		    {
		    	int k;
		    	for(k=idx ; k<table->len-1 ; k++)
		    	{
		    		table->users[k]=table->users[k+1];
		    	}
		    	table->len--;
		    }
	    }
	}
	
	perror("delete");
	return deleted_num;
	
}

///
/// Show the help messages
///
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";
    printf("%s", msg);
}

