/**********************************************************************************************************************
Project # 1
Yadi's O.S 
Created By:Yadiel R. Hernandez
Created Date:10/30/15
**********************************************************************************************************************/
#ifndef Yadi_OS_H
#define Yadi_OS_H

#include<iostream>
#include<sstream>
#include<string>
#include<queue>
#include<deque>
#include<limits>

using namespace std;

class Yadi_OS
{
public:

	Yadi_OS()
	{
		cout << "Welcome to Yadi's O.S" << endl;
		//always starts from 0
		nextPID = 0;
	}
	//SYS-GEN
	void sysgen()
	{
		while (true)
		{
			cout << "How many number # of (D - Disks)" << endl;
			if ((cin >> noDisks))
			{
				for (int i = 0; i < noDisks; i++)
				{
					diskList.push_back(Disk());
				}
				break; 
			}

			cin.clear();
			//the max possible numbers, handles 
			//invalid inputs
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input, please try again.\n" << endl;
		}

		
		while (true)
		{
			cout << "How many number # of (P - Printers)" << endl;
			if ((cin >> noPrinters))
			{
				for (int i = 0; i < noPrinters; i++)
				{
					printerList.push_back(Printer());
				}
				break;
			}
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input, please try again.\n" << endl;
		}
		while (true)
		{
			cout << "How much memory will the system have in (B - Bytes)" << endl;
			if ((cin >> noMemory))
			{
				//PCB is created & gets the memory value, 
				//points to the memory size 
				PCB *memory = new PCB;
				memory->memorySize = noMemory;
				memory->valid = false;
				pcbList.push_back(memory);
				break;
			}
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input, please try again.\n" << endl;
		}
	}
	//The running state of the program
	void running()
	{
		while(true)
		{
			//check  if queue is empty
			string command;
			if (readyQueue.empty())
			{
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << " CPU > ";
				//gets the string command for process 
				getline(cin, command);
				if (command == commandArrival)
				{
					unsigned int processMemory;
					while (true)
					{
						cout << "How much Memory does the process have in (B - Bytes)" << endl;
						if ((cin >> processMemory))
						{
							break;
						}
						cin.clear();
						cin.ignore(numeric_limits<streamsize>::max(), '\n');
						cout << "Invalid input, please try again.\n" << endl;
					}
					//finding the smallest & then finds the size 
					//of the smallest using best-fit
					unsigned int smallest = numeric_limits<unsigned int>::max();
					unsigned int smallestFit = numeric_limits<unsigned int>::max();
					for (unsigned int i = 0; i < pcbList.size(); i++)
					{
						if (!pcbList[i]->valid)
						{
							if (pcbList[i]->memorySize >= processMemory)
							{
								if (pcbList[i]->memorySize < smallestFit)
								{
									smallestFit = pcbList[i]->memorySize;
									smallest = i;
								}
							}
						}
					}

					if (smallest == numeric_limits<unsigned int>::max())
					{
						cout << "Not enough space.";
						continue;
					}

					//resize smallest. Memory -= process.memory
					PCB *smallestPCB = pcbList[smallest];
					smallestPCB->memorySize -= processMemory;
					if (smallestPCB->memorySize == 0)
					{
						delete smallestPCB;
						pcbList.erase(pcbList.begin() + smallest);
					}

					//new PCB, which will insert onto the queue.
					PCB *newPCB = new PCB;

					// gives it a PID.
					newPCB->PID = nextPID;
					nextPID++;

					newPCB->memorySize = processMemory;
					newPCB->valid = true;
					pcbList.insert(pcbList.begin() + smallest, newPCB);

					readyQueue.push(newPCB);
				}
				else if (command == commandSnapshot)
				{
					snapshot();
				}
				else if (command.size() > 1 && command[0] == commandPrinterInterrupt)
				{
					int number = atoi(command.substr(1).c_str());
					if (number == 0)
					{
						cout << "This is Invalid" << endl;
					}

					// Extract all the process in the printer.
					// Put all of them into the PCB ready queue.
					vector<PCB*> extractList = printerList[number - 1].extract();
					for (unsigned int i = 0; i < extractList.size(); i++)
					{
						readyQueue.push(extractList[i]);
					}
				}
				else
				{
					cout << "Please try again" << endl;
				}
			}
			else
			{
				PCB *pcb = readyQueue.front();
				readyQueue.pop();
				while (true)
				{
					cout << "Process " << pcb->PID << "> ";
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					getline(cin, command);
					if (command == commandTerminate)
					{
						// Terminate the process.
						cout << "Terminating..." << endl;
						//cout << "Please press enter" << endl;
						unsigned int index;
						for (index = 0; index < pcbList.size(); index++)
						{
							if (pcb == pcbList[index])
							{
								break;
							}
						}
						// Delete and remove it from the vector and resize closest (the one next to it, +1 or -1) free one.
						// Or, if there is no free one next to it, keep it just set valid false.
						bool leftValid = true;
						bool rightValid = true;

						// Case 1: On 2, delete 2 and 3, resize 1.
						// 1=I | 2=V | 3=I
						if (index > 0)
						{
							leftValid = pcbList[index - 1]->valid;
						}
						// Case 2: On 2, delete 2, resize 1.
						// 1=I | 2=V | 3=V
						if (index < pcbList.size() - 1)
						{
							rightValid = pcbList[index + 1]->valid;
						}
						// Case 3: On 2, delete 2, resize 3.
						// 1=V | 2=V | 3=I
						if (!leftValid)
						{
							// Case 4: On 1, keep 1, set invalid.
							// 1=I | 2=V | 3=V
							if (!rightValid)
							{
								pcbList[index - 1]->memorySize += pcbList[index + 1]->memorySize;
								delete pcbList[index + 1];
								pcbList.erase(pcbList.begin() + index + 1);
							}

							pcbList[index - 1]->memorySize += pcb->memorySize;
							delete pcb;
							pcbList.erase(pcbList.begin() + index);
						}
						else if (!rightValid)
						{
							pcbList[index + 1]->memorySize += pcb->memorySize;
							delete pcb;
							pcbList.erase(pcbList.begin() + index);
						}
						else
						{
							pcb->valid = false;
						}
						break;
					}
					else if (command == commandSnapshot)
					{
						snapshot();
					}
					else if (command.size() > 1 && command[0] == commandPrinter)
					{
						int number = atoi(command.substr(1).c_str());
						if (number == 0)
						{
							// ERROR
							cout << "This is Invalid" << endl;
							continue;
						}
						printerList[number - 1].add(pcb);

						//ask user for filename, & ask user for r/w.
						cout << "Please enter the file name" << endl;
						getline(cin, pcb->filename);
						cout << "What is the file size" << endl;
						cin >> pcb->filelength;
						pcb->readOrWrite = 'w';
						break;
					}
					else if (command.size() > 1 && command[0] == commandDisk)
					{
						int number = atoi(command.substr(1).c_str());
						if (number == 0)
						{
							cout << "This is Invalid" << endl;
							continue;
						}
						diskList[number - 1].add(pcb);

						cout << "Please enter the file name" << endl;
						getline(cin, pcb->filename);
						cout << "What is the file size" << endl;
						cin >> pcb->filelength;
						cout << " Which one would you like to do, read or write " << endl;
						cin >> pcb->readOrWrite;
						while (pcb->readOrWrite != 'r' && pcb->readOrWrite != 'w')
						{
							cout << "Error";
							cin >> pcb->readOrWrite;
						}
						break;
					
					}
					else
					{
						cout << "Invalid common to the process";
						continue;
					}
				}
			}
		}
	}

	void snapshot()
	{
		// Ask if the user wants r, p or d.
		string command;
		while (true)
		{
			cout << "Which would you like to do :: ( r | p | d | m) " << endl;
			getline(cin, command);
			if (command == commandSnapshotShow)
			{
				//command snapshot r
				//queue is copied, & loop until empty, and pop everything you look at.
				std::queue<PCB*> readyQueueCopy = readyQueue;
				while (!readyQueueCopy.empty())
				{
					cout << " PID " << readyQueueCopy.front()->PID << endl;
					readyQueueCopy.pop();
				}
				break;
			}
			else if (command == commandSnapshotMemory)
			{
				// PID 1, 2 bytes long, 0.
				// Unused, 1 btyte long, 2.
				// PID 4, 4 bytes long, 3.
				unsigned int offset = 0;
				for (unsigned int i = 0; i < pcbList.size(); i++)
				{
					cout << "Offset " << offset;
					if (pcbList[i]->valid)
					{
						cout << " PID " << pcbList[i]->PID;
					}
					else
					{
						cout << "Unused ";
					}
					cout << "of length " << pcbList[i]->memorySize << endl;
					offset += pcbList[i]->memorySize;
				}
			}
			else if (command == commandSnapshotPrinter)
			{
				// command snapshot printer p
				// Loop through all the printer.
				// Loop through the vector, PID, filename, etc.
				for (unsigned int i = 0; i < printerList.size(); i++)
				{
					vector<PCB*> queue = printerList[i].get();
					cout << "Printer " << i + 1 << ":" << std::endl;
					for (unsigned int j = 0; j < queue.size(); j++)
					{
						cout << "    " << queue[j]->PID << " " << queue[j]->filename << " " << queue[j]->filelength;
						cout << " " << queue[j]->readOrWrite << endl;
					}
				}
				break;

			}
			// command snapshot disk d
			// Loop through all the disk.
			// Loop through the vector, PID, filename, etc.
			else if (command == commandSnapshotDisk)
			{
				for (unsigned int i = 0; i < diskList.size(); i++)
				{
					vector<PCB*> queue = diskList[i].get();
					cout << " Disk " << i + 1 << ":" << std::endl;
					for (unsigned int j = 0; j < queue.size(); j++)
					{
						cout << "    " << queue[j]->PID << " " << queue[j]->filename << " " << queue[j]->filelength;
						cout << " " << queue[j]->readOrWrite << endl;
					}
				}
				break;

			}
			else
			{
				cout << "please try again" << endl;
			}
		}
	}
		~Yadi_OS()
		{
			//loop to dealocate memory, and prevents leakage 
			for (unsigned int i = 0; i < pcbList.size(); i++)
			{
				delete pcbList[i];
			}
		}

	private:

		//a process control block (PCB)
		//for each process
		struct PCB
		{
			//generates & identifies the 
			//different process  	
			unsigned int PID;
			//creates a memory size  
			unsigned int memorySize;
			//file Name 
			string filename;
			//read or Write
			char readOrWrite;
			//file length
			unsigned int filelength;
			//boolean cheacks if its valid
			bool valid;
		};
		
		//stores the memory 
		vector<PCB*> pcbList;

		//Class called device for disk & printers 
		class Printer
		{

		private:

			vector<PCB*> processes;

		public:
			void add(PCB *pcb)
			{
				processes.push_back(pcb);
			}

			vector<PCB*> extract()
			{
				vector<PCB*> list = processes;
				processes.clear();
				return list;
			}
			//The Copy to loop through
			vector<PCB*> get()
			{
				return processes;
			}
		};

		vector<Printer> printerList;
		
		class Disk
		{

		private:

			vector<PCB*> processes;

		public:
			void add(PCB *pcb)
			{
				processes.push_back(pcb);
			}

			vector<PCB*> extract()
			{
				vector<PCB*> list = processes;
				processes.clear();
				return list;
			}
			//The Copy to loop through
			vector<PCB*> get()
			{
				return processes;
			}
		};

		vector<Disk> diskList;

		//system Variable &  Interrupts & System calls 
		const string commandArrival = "A";
		const string commandSnapshot = "S";
		const string commandTerminate = "t";
		//processes ReadyQueue
		const string commandSnapshotShow = "r";
		//printer/disk - Snapshot
		const string commandSnapshotPrinter = "p";
		const string commandSnapshotMemory = "m";
		const string commandSnapshotDisk = "d";
		//printer for a character 
		const char commandPrinter = 'p';
		const char commandPrinterInterrupt = 'P';
		//disk for a character 
		const char commandDisk = 'd';

		//number of PC devices
		int noDisks;
		int noPrinters;
		int noMemory;

		//gets the next PID 
		unsigned int nextPID;
		queue<PCB*> readyQueue;

	};

#endif // Yadi_OS_H