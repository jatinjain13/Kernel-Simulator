// Jatin Jain - 101184197
//Assignment 3 SYSC 4001A
//November 8, 2022
//To run this file in a windows terminal open this folder and run/type "test1.bat" or "test2.bat"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

//PCB structure that holds information for the whole process
typedef struct pcb {
   	int pid; //process identifier
   	int arrival; //arrival time 
	int totalCPU; //total cpu time required, subtract from this each cycle when running
	int IOfreq; //IO frequency 
	int IOduration; //IO duration
	int exitIO; //at what elapsed time IO is complete
	int priority; //Priority given read CSV 
	int cpu_arrival; // When processor arrives in CPU
	int time_until_IO;//tracks when process must go for IO
	int memory; //How much memory a process occupies
	int start_address; //Address/point in memory that processes first instruction is found
	struct pcb *next; //pointer to next pcb in the queue
} pcb_t;

//struct used to store info about each queue
typedef struct {
	pcb_t *head; //first in queue
	pcb_t *tail; //last in queue
	int size; //number of processes in the queue
} queue_t;

//struct that holds info about a single partition
typedef struct {
	int space; //memory in a partition
	pcb_t *allocated_pcb; // process that is in this memory partition	
	int start_address; //first instruction in memory
}partition_t;

//prev, curr and pcb_to_insert will be used to have a sort and remove functions from the middle of a queue
pcb_t *prev;
pcb_t *curr;
pcb_t *pcb_to_insert; 

//Metrics
int total_pcb = -1; //tracks total number of processes
int total_terminated =0; //tracks total number of processes that were terminated
int total_turnaround_time=0; //tracks total turnaround time
int total_wait_time=0; //tracks total wait time 
int total_response_time=0; //tracks total repsonse time



//creates a queue that will be used for job, running, waiting and ready
queue_t *create_queue(void) {
    queue_t *queue = malloc(sizeof(queue_t));
    
    queue->head = NULL;
	queue->tail = NULL;			
    queue->size = 0;

    return queue;
}



//used for debugging as it prints out the queue contents in order
void print_list(queue_t *queue)
{
    if (queue->head == NULL) {
        printf("empty list \n");
        return; 
    }

    pcb_t *curr;

    for (curr = queue->head; curr->next != NULL; curr = curr->next) {
        printf("%d -> ", curr->pid);
    }

    /* Print the last node. */
    printf("%d \n", curr->pid);
}

//Delete function used to remove from head of a queue
void delete(queue_t *queue){
	if(queue->head == NULL){ //used for error handling
		printf("Queue is empty");
		assert(false);
	}

	pcb_t *pcb_del = queue->head; 

	//remove from head and assign new head
	queue->head = queue->head->next;
	pcb_del->next = NULL;
	queue->size--;

	//if there is no head
	if (queue->head==NULL){
		queue->tail=NULL;	
			
	}
}


//Used to ensure that a queue has no processes (Used for running)
void empty(queue_t *queue){
	queue->head->next = NULL;
	queue->head = NULL;
	queue->tail = NULL;
	queue->size =0;
}

//insert process into a queue from tail
void insert(queue_t *queue, pcb_t *pcb){
	//case 1 queue is empty
	if (queue->head == NULL){ 
		queue->head = pcb;
		queue->tail = queue->head;
	}

	//general case add new tail 
	else {
		queue->tail->next = pcb;
		queue->tail = pcb;
	}

	queue->size +=1;

	}

//return head pointer of queue
pcb_t *head(queue_t* queue){
	
	return queue->head;
	
	}

//create a new pcb (used after reading a file)
pcb_t *create_pcb(int pid, int arrival, int totalCPU, int IOfreq, int IOduration, int priority, int memory) {
    pcb_t *pcb = malloc(sizeof(pcb_t)); //allocate room for a pcb
    assert(pcb != NULL);

	//Read all input and store into pcb needed
    pcb->pid = pid;
    pcb->arrival = arrival;
    pcb->totalCPU = totalCPU;
    pcb->IOfreq = IOfreq;
	pcb->time_until_IO = IOfreq-1; //At the beginning process is not admitted into CPU (-1 to account for time in CPU when first admitted)
    pcb->IOduration = IOduration;
	pcb->priority = priority;
    pcb->next = NULL; 
	pcb->cpu_arrival = -1; //Keep CPU arrival as a negative number to distinguish when a pcb occupies CPU for first time
	pcb->memory = memory;
    return pcb;
}

//copies info of pcb into new pcb (used for troubleshooting)
pcb_t *copy(pcb_t *pcb) {
    pcb_t *newpcb = malloc(sizeof(pcb_t));
    assert(newpcb != NULL);

    newpcb->pid = pcb->pid;
    newpcb->arrival = pcb->arrival;
    newpcb->totalCPU = pcb->totalCPU;
    newpcb->IOfreq = pcb->IOfreq;
	newpcb->time_until_IO = pcb->time_until_IO; //At the beginning process is not admitted into CPU (-1 to account for time in CPU when first admittedw)
    newpcb->IOduration = pcb->IOduration;
	newpcb->priority = pcb->priority;
    newpcb->next = NULL; 
	newpcb->cpu_arrival = pcb->cpu_arrival;
	newpcb->memory = pcb->memory;
    return newpcb;
}

//Stores data from csv into job queue
void load(char file_name[], queue_t *job){

	//local variables used to hold pcb content temporarily
    int pid;
    int arrival;
    int totalCPU;
    int IOfreq;
    int IOduration;
    int row = 0; //row of csv
    int column = 0; //column of csv
	int priority;
	int memory;

	FILE *file = fopen(file_name, "r");

    if (!file){
        printf("Error opening file\n");
    }


    else{
        char line[1000];  //Holds content of line
	 while (fgets(line,1000,file)){
			
            column = 0;
            row++; //Move to next row
			total_pcb++; //Increase number of pcb fed
            char* data = strtok(line, ",");
            while (data){
                if (row == 1); //skip titles
				
                else if (column == 0){
                    pid = atoi(data);
          
                }

                else if (column == 1){
                    arrival = atoi(data);
              
                }


                else if (column == 2){
                    totalCPU = atoi(data);

                }

                else if (column == 3){
                    IOfreq = atoi(data);
                    
                }

                else if (column == 4){
                    IOduration = atoi(data);
				}

				else if (column == 5){
					priority = atoi(data);
				}
				
				else if (column == 6){
					memory = atoi(data);	
					
				
					//add to job queue
                	insert(job, create_pcb(pid, arrival, totalCPU, IOfreq, IOduration, priority, memory));
    			}    

                data = strtok(NULL, ",");
                column++;
				
            }
        }
	}fclose(file);
	
	}

//Stores data from csv into job queue if memory is selected
//Seperate loader to ensure that a process does not exceed space of any partition
void memory_load(char file_name[], queue_t *job, int space1, int space2, int space3, int space4){

	//local variables used to hold pcb content temporarily
	
    int pid;
    int arrival;
    int totalCPU;
    int IOfreq;
    int IOduration;
    int row = 0; //row of csv
    int column = 0; //column of csv
	int priority;
	int memory;

	FILE *file = fopen(file_name, "r");

    if (!file){
        printf("Error opening file\n");
    }


    else{
        char line[1000];  //Holds content of line
	 while (fgets(line,1000,file)){
			
            column = 0;
            row++; //Move to next row
			total_pcb++; //Increase number of pcb fed
            char* data = strtok(line, ",");
            while (data){
                if (row == 1); //skip titles
				

                else if (column == 0){
                    pid = atoi(data);
          
                }

                else if (column == 1){
                    arrival = atoi(data);
              
                }


                else if (column == 2){
                    totalCPU = atoi(data);

                }

                else if (column == 3){
                    IOfreq = atoi(data);
                    
                }

                else if (column == 4){
                    IOduration = atoi(data);
				}

				else if (column == 5){
					priority = atoi(data);
				}
				
				else if (column == 6){
					memory = atoi(data);	
					
					//only insert into job queue if memory size does not exceed space of all individual partitions
					if (memory <= space1 || memory <= space2 || memory <= space3 || memory <= space4){        		
						insert(job, create_pcb(pid, arrival, totalCPU, IOfreq, IOduration, priority, memory));
					}

					else{
						printf("process %d's size is greater than all partitions\n", pid);
						total_pcb--; //decrement total pcb since this process is not going into job queu	
					}
				}
    		    

            data = strtok(NULL, ",");
            column++;
			}
     	}
        
	}fclose(file);
	
}


 
int main(int argc, char *argv[])
{
	int scheduling; // Used to determine which scheduling algo is used (0: FCFS, 1: Priority based 2: RR)
	int time_quantum; // time quantum used for RR scheduling
	int total_used_memory = 0; //Total memory consumption of processes currently in memory
	int total_free_memory = 0; //without internal fragmentation
	int total_usable_memory = 0; //with internal fragmentation
	int used_partitions = 0; //number of partitions used
	int cpu_utilization = 0; //used for CPU utilization metrics
	//choose memory option and which set up for memory 
	//(0: No, 
	// 1: Yes (take memory partitions from Batch file),  
	int memory_type; 

	//Take values from batch file
	scheduling = atoi(argv[3]);
	memory_type = atoi(argv[4]);
	time_quantum = atoi(argv[5]);

	int time_elapsed = 0; //timer for overal system
	int time_in_cpu = 1; //time that a process is in the cpu for current period

 	queue_t *job = create_queue();
    queue_t *ready = create_queue();
    queue_t *running = create_queue();
    queue_t *waiting = create_queue();
 
	if (memory_type == 0){
		load(argv[1], job); //info from csv into job queue
	}

	if (memory_type ==1 || memory_type==2){
		memory_load(argv[1], job, atoi(argv[6]), atoi(argv[7]), atoi(argv[8]), atoi(argv[9])); //info from csv into job queue		
	}

	FILE* output_file = fopen(argv[2],"w+");

	partition_t memory[4];

	//Create memory partitions 
	if (memory_type == 1){
		
		//Amount of space is taken from batch file
		memory[0].space = atoi(argv[6]);
		memory[1].space = atoi(argv[7]);
		memory[2].space = atoi(argv[8]);
		memory[3].space = atoi(argv[9]);

		//Next memory address starts at where the last ended + its own size
		memory[0].start_address = 0;
		memory[1].start_address = memory[1].space + memory[0].start_address;
		memory[2].start_address = memory[2].space + memory[1].start_address;
		memory[3].start_address = memory[3].space + memory[2].start_address;


		memory[0].allocated_pcb = NULL;
		memory[1].allocated_pcb = NULL;
		memory[2].allocated_pcb = NULL;
		memory[3].allocated_pcb = NULL;

		total_usable_memory = memory[0].space + memory[1].space + memory[2].space + memory[3].space;


	}

	//This is used if the user wants pre defined partitions based off of scenario 2 of Assignment 2
	else if (memory_type == 2){
		
		memory[0].space = 300;
		memory[1].space = 300;
		memory[2].space = 350;
		memory[3].space = 50;

		memory[0].start_address = 0;
		memory[1].start_address = 300;
		memory[2].start_address = 600;
		memory[3].start_address = 950;


		memory[0].allocated_pcb = NULL;
		memory[1].allocated_pcb = NULL;
		memory[2].allocated_pcb = NULL;
		memory[3].allocated_pcb = NULL;

		total_usable_memory = 1000;
	}



	fprintf(output_file, "Time of transition,PID,Old State,New State\n");

	//PROGRAM LOOP
	while(total_pcb != total_terminated){
		
		//Increase wait time for each process in ready
		total_wait_time += ready->size;

		//NEW -> READY
		//When memory is chosen, the new to ready process is based off memory space
		if (memory_type == 1 || memory_type == 2){
			curr = job->head;
			prev = NULL;
			//Loop will check if next process in job queue has it arrival time
			while(curr != NULL && curr->arrival <= time_elapsed){

				//Loop will check each partition for space
				for (int i = 0; i < 4; i++) {

					//printf("Checking Memory for Process %d...\n", curr->pid);
					//if (memory[i].allocated_pcb !=NULL){
						//printf ("Partition %d is allocated by %d\n", i+1, memory[i].allocated_pcb->pid);	
					//}

					//if there is enough space in a partition
					if (memory[i].allocated_pcb == NULL && memory[i].space >= curr->memory){
							used_partitions += 1; //number of used partition increases
							memory[i].allocated_pcb = curr; //allocate pcb to partition
							total_used_memory += curr->memory; //increased used memory and decrease usable memory
							total_usable_memory -= memory[i].space;
							curr->start_address = memory[i].start_address;

							//Output to terminal 
							printf("MEMORY PARTITION %d HAS BEEN ALLOCATED TO PROCESS %d\n", i+1, curr->pid);
							printf("Free memory portions: %d\n", 4 - used_partitions);
							printf("Total Used Memory: %d\n", total_used_memory);
							printf("Total Free Memory: %d\n", 1000-total_used_memory);
							printf("Total Usable Memory: %d\n", total_usable_memory);
							printf("-----------------------------\n");
							
						    //If the first process in the job queue it is a simple delete and insert
							if (prev==NULL){
								insert(ready, job->head);
								fprintf(output_file, "%d,%d,NEW,READY\n", time_elapsed, curr->pid);
								delete(job);
							}

							//If it is not the first process, a sort from the middle of the queue must be done
							else{
								prev->next = curr->next;
								curr->next = NULL;
								job->size -= 1;
								insert(ready, curr);
								fprintf(output_file, "%d,%d,NEW,READY\n", time_elapsed, curr->pid);
								//Assign a new tail
								curr = job->head;
								while (curr->next !=NULL){
									curr=curr->next;
								}
								job->tail = curr;	
							}
							i=4; //Used to end the loop

					}
				}
				prev = curr;
				curr = curr->next;
				
			}
		}	
		//NO MEMORY MANAGEMENT
		else{
			while(job->head != NULL && job->head->arrival <= time_elapsed){
			 
			insert(ready, job->head);
			fprintf(output_file,"%d,%d,NEW,READY\n", time_elapsed, job->head->pid);
			delete(job);
			}
			
		}
		
		//When a process is occupying the CPU check for context switched
		if(running->head != NULL){
			cpu_utilization++; //a process is in the CPU
			//RUNNING -> TERMINATED
			if (running->head->totalCPU == 0){ 
				//De allocate if memory is involved
				if (memory_type == 1 || memory_type == 2){
					for (int i = 0; i < 4; i++) {
						//Find which partition points to this process then update memory 						
						if (memory[i].allocated_pcb == running->head || running->head->start_address == memory[i].start_address){
							used_partitions -= 1;
							total_used_memory -= running->head->memory;																																																																																														
							total_usable_memory += memory[i].space;
							running->head->start_address = 0;
							printf("MEMORY HAS BEEN DE-ALLOCATED FROM PROCESS %d\n", running->head->pid);
							printf("Free memory portions: %d\n", 4 - used_partitions);
							printf("Total Used Memory: %d\n", total_used_memory);
							printf("Total Free Memory: %d\n", 1000-total_used_memory);
							printf("Total Usable Memory: %d\n", total_usable_memory);
							printf("-----------------------------\n");
							memory[i].allocated_pcb = NULL;
						}	
					
					
					}
				}
				fprintf(output_file,"%d,%d,RUNNING,TERMINATED\n", time_elapsed, running->head->pid);
				total_turnaround_time += time_elapsed - running->head->arrival; //Turn around time for terminated process
				free(running->head); //free since it is no longer needed and makes room for other data
				running->head = NULL;
				total_terminated++;
				
			}
			
			//RUNNING -> WAIT
			else if ((running->head->time_until_IO) == 0){
				fprintf(output_file,"%d,%d,RUNNING,WAITING\n", time_elapsed, running->head->pid);
				running->head->exitIO = time_elapsed + running->head->IOduration; //when process must leave IO
				running->head->time_until_IO = running->head->IOfreq - 1;				//Reset
				insert(waiting, running->head);
				empty(running); //empty running since there should only be 1 process at a time
			}
						
			//Round Robin Scheduling RUNNING->READY
			//Preemptively free process from CPU if time quantum is reached
			else if (scheduling == 2 && ((time_elapsed - running->head->cpu_arrival) >= time_quantum))  {
                fprintf(output_file,"%d,%d,RUNNING,READY\n", time_elapsed, running->head->pid);
				running->head->time_until_IO-=1;
				insert(ready, running->head);
				delete(running);
				
            } 
			

			//Running has not hit total CPU time or IO frequency
			else{
				running->head->time_until_IO -=1;
				running->head->totalCPU -= 1;
			}
			
		}
		
		
		//When a process is receving IO
		if (waiting->head != NULL){
			curr = waiting->head;
			prev=NULL;
			//WAITING -> READY
			//Loop will check each process if it has completed IO
			while(curr != NULL){
				if ((curr->exitIO) == time_elapsed){  // Can do <= for quick fix

					fprintf(output_file,"%d,%d,WAITING,READY\n", time_elapsed, curr->pid);

					if (prev == NULL){
						insert(ready, waiting->head);
						delete(waiting);
					}

					else{
					
					prev->next = curr->next;
					curr->next = NULL;				
					waiting->size -= 1;
					insert(ready, curr);


					//Assign a new tail
					curr = waiting->head;
					while (curr->next !=NULL){
						curr=curr->next;
					}
					waiting->tail = curr;
		
					}
					
				
				}

				prev = curr;
				curr = curr->next;
			
			}
		}
		
		
		//Repeat to ensure another process can leave wait this loop is infinite making it take more time
		if (waiting->head != NULL){
			curr = waiting->head;
			prev=NULL;
			//WAITING -> READY
			while(curr != NULL){
				if ((curr->exitIO) <= time_elapsed){  // Can do <= for quick fix

					fprintf(output_file,"%d,%d,WAITING,READY\n", time_elapsed, curr->pid);

					if (prev == NULL){
						insert(ready, waiting->head);
						delete(waiting);
					}

					else{
					prev->next = curr->next;
					curr->next = NULL;				
					waiting->size -= 1;
					insert(ready, curr);


					//Assign a new tail
					curr = waiting->head;
					while (curr->next !=NULL){
						curr=curr->next;
					}
					waiting->tail = curr;
		
					}
					
				
				}

				prev = curr;
				curr = curr->next;
			
			}
		}
		
		

		//READY -> RUNNING
		if ((ready->head != NULL) && (running->head == NULL)){
	
			//FCFS or RR
			if (scheduling == 0 || scheduling == 2 || scheduling == 3 || scheduling == 4){ //initially memory was going to be scheduling 3 or 4 so not needed 
				insert(running, ready->head);
				fprintf(output_file,"%d,%d,READY,RUNNING\n", time_elapsed, ready->head->pid);
				delete(ready);
			}

			// PRIORITY SCHEDULING
			else if (scheduling == 1){
				//used to find any process which has highest priority (lowest number)
				curr = ready->head; //head of ready in queue
				pcb_to_insert = ready->head;
				prev=NULL; //previous process to the one you will delete
				while(curr->next != NULL){ //while there are contents in ready queue
					if ((curr->next->priority) < (pcb_to_insert->priority)){ //higher priority found
						pcb_to_insert = curr->next; //assign insertable pcb to highest priority
						prev = curr; //prev is process before the insertable used for removing from middle of list
					}
					curr = curr->next; 
				}

				if (prev!=NULL){ //when the highest priority process is not the head process, remove from middle
					prev->next=pcb_to_insert->next; 
					pcb_to_insert->next = NULL;
					ready->size -=1;

					//Assign a new tail
					curr = ready->head;
					while (curr->next !=NULL){
						curr=curr->next;
					}
					ready->tail = curr;
				}

				else{ //head of ready is removed using delete function
					delete(ready);
				}
				insert (running, pcb_to_insert);
				fprintf(output_file,"%d,%d,READY,RUNNING\n", time_elapsed, pcb_to_insert->pid);
			} 
			
			//when a process occupies cpu for first time, it receives its first response
			if (running->head->cpu_arrival == -1){
				total_response_time += time_elapsed - running->head->arrival;		
			}

			running->head->cpu_arrival = time_elapsed; //set new arrival time
			running->head->totalCPU--;
			
		}
		
		


		//Update Timers
		time_elapsed +=1;
	
	}
	fclose(output_file);
	free (job);
	free (running);
	free (ready);
	free (waiting);
	printf("CPU UTILIZATION: %.2lf %% \n", (double)((cpu_utilization+1)*100/time_elapsed));
	printf("THROUGHPUT: %.2lf process/s\n", (double) total_pcb/time_elapsed*1000);
	printf("AVG TURNAROUND TIME: %.2lf ms/process\n", (double) total_turnaround_time / total_pcb);
	printf("AVG WAIT TIME: %.2lf ms/process\n", (double) total_wait_time/total_pcb);
	printf("AVG RESPONSE TIME: %.2lf ms/process\n", (double) total_response_time/total_pcb);

	printf("Your results are stored in the output file\n");
	return 0;



}

//  ./name "testcase1" "testcase2"
