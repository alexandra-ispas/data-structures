//ISPAS Alexandra-Petrina 312CD
#include <stdio.h>
#include "WearhouseManager.h"


Package *create_package(long priority, const char* destination){
	// TODO 3.1
	Package* p = (Package*)malloc(sizeof(Package));
	//eroare locare
	if(p == NULL) exit(1);

	p->priority = priority;
	
	if(destination != NULL){

		p->destination = strndup(destination, strlen(destination) + 1);
		//eroare locare
		if(p->destination == NULL){
			free(p);
			exit(1);
		}
	}

	else
		 p->destination = NULL;
	return p;
}

void destroy_package(Package* package){
	// TODO: 3.1
	if(package->destination != NULL)
		free(package->destination);

	free(package);
}

Manifest* create_manifest_node(void){
	// TODO: 3.1
	Manifest* m = (Manifest*)malloc(sizeof(Manifest));
	if(m == NULL) exit(1);

	m->next = NULL;
	m->prev = NULL;
	m->package = NULL;

	return m;
}

void destroy_manifest_node(Manifest* manifest_node){
	//scot pachetul
	if (manifest_node->package != NULL) 
		destroy_package(manifest_node->package);

	//modific legaturile
	if (manifest_node->next != NULL) 
		manifest_node->next->prev = manifest_node->prev;

	if (manifest_node->prev != NULL) 
		manifest_node->prev->next = manifest_node->next;

	//eliberez nodul
	free(manifest_node);
}


Wearhouse* create_wearhouse(long capacity){
	// TODO: 3.2
	//guard
	if(capacity == 0) return NULL;

	Wearhouse* w = (Wearhouse*)malloc(sizeof(Wearhouse));
	if(w == NULL) exit(1);

	w->size = 0;
	w->capacity = capacity;

	w->packages = (Package**)malloc((capacity)*sizeof(Package));
	if(w->packages == NULL) exit(1);

	for( long i = 0; i < w->capacity; i++) //initializeaza pachetele
		w->packages[i] = NULL;

	return w;
}

Wearhouse *open_wearhouse(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Wearhouse *w = NULL;


	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	if((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		w = create_wearhouse(atol(token));

		free(line);
		line = NULL;
		len = 0;
	}

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		long priority = atol(token);
		token = strtok(NULL, ",\n ");
		Package *p = create_package(priority, token);
		w->packages[w->size++] = p;

		free(line);
		line = NULL;
		len = 0;
	}

	free(line);


	fclose(fp);
	return w;

	file_open_exception:
	return NULL;
}

long wearhouse_is_empty(Wearhouse *w){
	// TODO: 3.2
	//sunt 0 pachete
	return (w->size == 0);
}

long wearhouse_is_full(Wearhouse *w){
	// TODO: 3.2
	//are nr maxim de pachete
	return (w->size == w->capacity);
}

long wearhouse_max_package_priority(Wearhouse *w){
	// TODO: 3.2
	//initializez
	long max = w->packages[0]->priority;

	for( long i = 1; i < w->size; i++)

		if( max < w->packages[i]->priority)
			max = w->packages[i]->priority;

	return max;
}

long wearhouse_min_package_priority(Wearhouse *w){
	// TODO: 3.2

	long min = w->packages[0]->priority;

	for( long i = 1; i < w->size; i++)

		if( min > w->packages[i]->priority)
			min = w->packages[i]->priority;

	return min;
}


void wearhouse_print_packages_info(Wearhouse *w){

	for(long i = 0; i < w->size; i++){
		printf("P: %ld %s\n",
				w->packages[i]->priority,
				w->packages[i]->destination);
	}
	printf("\n");
}

void destroy_wearhouse(Wearhouse* wearhouse){
	// TODO: 3.2

	for( long i = 0; i < wearhouse->size; i++)
		//distrug pachetele
		if(wearhouse->packages[i] != NULL)
			destroy_package(wearhouse->packages[i]);

	free(wearhouse->packages);

	free(wearhouse);
}


Robot* create_robot(long capacity){
	// TODO: 3.2

	Robot* r = (Robot*)malloc(sizeof(Robot));
	if(r == NULL) exit(1);

	r->size = 0;
	r->capacity = capacity;
	//fixez legaturile
	r->next = NULL;

	r->manifest = NULL;

	return r;
}

int robot_is_full(Robot* robot){
	// TODO: 3.2

	return (robot->size == robot->capacity);
}

int robot_is_empty(Robot* robot){
	// TODO: 3.2

	return (robot->size == 0);
}

Package* robot_get_wearhouse_priority_package(Wearhouse *w, long priority){
	// TODO: 3.2
	//parcurg toate pachetele
	for( long i = 0; i < w->size; i++ )
		//daca are prioritatea dorita, il returnez
		if(w->packages[i]->priority == priority)

			return w->packages[i];

	return NULL;
}

void robot_remove_wearhouse_package(Wearhouse* w, Package* package){
	// TODO: 3.2
	long i = 0;
	//parcurg pachetele
	for(i = 0; i < w->size; i++)
		//daca il gasesc
		if(w->packages[i] == package){
			//il scot
			for(long j = i; j< w->capacity; j++)
				w->packages[j] = w->packages[j+1];
			//modific numarul de pachete
			if(w->size > 0)
				w->size--;
		}
				
			
}

void robot_load_one_package(Robot* robot, Package* package){
	// TODO:  3.2

	//nu mai am unde sa adaug
	if( robot_is_full(robot) ) return;

	//creez nodul ce urmeaza sa-l introduc
	Manifest* m = create_manifest_node(); 
	m->package = package;
	m->next = NULL;
	m->prev = NULL;

	//adaug intr-un robot gol
	if(robot_is_empty(robot)){
		robot->size++;
		robot->manifest = m;
		return;
	}
	//cresc marimea
	robot->size++;

	//iterator
	Manifest* aux = robot->manifest;

	while(aux != NULL ){

		//daca are pachet
		if(aux->package != NULL){

			//au prioritate diferita
			if(package->priority != aux->package->priority){

				//daca il pun primul in lista
				if((package->priority > aux->package->priority && aux->prev == NULL)){

					m->next = aux;
					aux->prev = m;
					robot->manifest = m;

					return;
				}
					//daca il pun ultimul
				else if((package->priority < aux->package->priority && aux->next == NULL)){
						m->prev = aux;
						aux->next = m;
						return;
					}
					//daca e la mijloc
				else if(aux->next != NULL){

					if(package->priority > aux->next->package->priority && package->priority < aux->package->priority){

						m->next = aux->next;
						m->prev = aux;
						aux->next->prev = m;
						aux->next = m;

						return;
					}
				}
			}
			else{
				//prioritatile sunt egale, compar destinatiile
				int x = strcmp(package->destination, aux->package->destination);

				//il pun primul
				if(x < 0 && aux->prev == NULL ){  

					m->next = aux;
					aux->prev = m;
					robot->manifest = m;

					return;
				}
				//il pun ultimul
				else if(x > 0 && aux->next == NULL){ 

					m->prev = aux;
					aux->next = m;

					return;
				}
				//daca trebuie pus dupa iterator
				else if(x > 0){
						//verific daca respecta regula cu urmatorul pachet

					if(aux->next != NULL && strcmp(package->destination, aux->next->package->destination) < 0){ 

						m->next = aux->next;
						m->prev = aux;
						aux->next->prev = m;
						aux->next = m;

						return;
					}
				}

				//daca trebuie pus inaintea iteratorului
				else if(x < 0){
					//verific daca respecta regula cu pachetul de dinainte
					if(aux->prev != NULL && strcmp(package->destination, aux->prev->package->destination) > 0){

						m->next = aux;
						m->prev = aux->prev;
						aux->prev->next = m;
						aux->prev = m;

						return;
					}
				}
				//destinatiile sunt identice
				else if(x == 0){ 
					//daca il adaug la sfarsit
					if(aux->next == NULL){
						m->prev = aux;
						aux->next = m;
						return;
					}
					//daca il adaug la mijloc
					else{

						m->next = aux->next;
						m->prev = aux;
						aux->next->prev = m;
						aux->next = m;

						return;
					}
				}

			}

		}
		//iterez
		aux = aux->next;
	}
}

long robot_load_packages(Wearhouse* wearhouse, Robot* robot){
	// TODO: 3.2

	long p;

	Package* pac; //iterator

	//initializez numarul de pachete adaugate
	long i = 0;
	//pot adauga daca mai am loc in robot si mai am pachete in magazie
	while( robot_is_full(robot) == 0 && wearhouse_is_empty(wearhouse) == 0){  
		//caut prioritatea maxim
		p = wearhouse_max_package_priority(wearhouse); 
		//gasesc pachetul cu acea prioritate
		pac = robot_get_wearhouse_priority_package(wearhouse, p); 
		//adaug pachetul in robot
		robot_load_one_package(robot, pac); 
		//scot pachetul din magazie                     
		robot_remove_wearhouse_package(wearhouse, pac);	
		//creste numarul pachetelor adaugate
		i++;							
	}

	return i;
	
}

Package* robot_get_destination_highest_priority_package(Robot* robot, const char* destination){
	// TODO: 3.2

	long max = 0, i = 0;

	Package* p;
	//iterez
	Manifest* m = robot->manifest;
	//parcurg atata timp cat am elemente in robot
	while(i < robot->size ){

		Package* aux = m->package;
		//compar destinatiile
		if(strcmp(aux->destination, destination) == 0){
			//compar prioritatile
			if(max < aux->priority){

				max = aux->priority;
				p = aux;
			}
		}
		//iterez
		m = m->next;
		i++;
	}
	return p;
}

void destroy_robot(Robot* robot){
	// TODO: 3.2

	Manifest* aux;
	//distrug toate manifest-urile din robot
	while(!robot_is_empty(robot)){

		aux = robot->manifest;
		robot->manifest = robot->manifest->next;
		destroy_manifest_node(aux);
		robot->size--; //scade dimensiunea robotului
	}
	//eliberez memoria pentru robot
	free(robot);
}

void robot_unload_packages(Truck* truck, Robot* robot){
	// TODO: 3.3

	Truck* t = truck;
	//daca robotul e gol, nu am ce sa adaug
	if(robot_is_empty(robot)) return;
	//iterez manifest-urile robotului
	Manifest* m = robot->manifest;
	//daca nu are manifest-uri sau truck-ul este plin, ies
	if(m == NULL || truck_is_full(truck)) return; 
	//atata timp cat pot adauga
	while(m != NULL && !truck_is_full(truck)){
		//daca au aceeasi destinatie
		if(strcmp(t->destination, m->package->destination) == 0){

			//scot din robot

			//daca este primul
			if(m->prev != NULL){
				m->prev->next = m->next;
			}
			else
				//este la mijloc
				robot->manifest = m->next;
			//daca este ultimul
			if(m->next != NULL)

				m->next->prev = m->prev;

			//il mut pe m intr-o alta variabila pentru a nu modifica robotul
			Manifest* new = m;
			//iterez
			m = m->next;
			//scade marimea robotului
			if(robot->size > 0){
				robot->size--;
			}

			//adaug in truck

			//daca este gol
			if(truck_is_empty(truck)){

				t->manifest = new;
				t->manifest->next = NULL;
				
			}
			else{
				t->manifest->prev = new;
				t->manifest->prev->next = t->manifest;
				t->manifest = new;
				new->prev = NULL;
				
			}
			//creste marimea truck-ului
			t->size++;
		}else
			//nu au aceeasi destinatie deci caut alt manifest
			m = m->next;	
	}
	return;
}

// Attach to specific truck
int robot_attach_find_truck(Robot* robot, Parkinglot *parkinglot){
	int found_truck = 0;
	long size = 0;
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	Manifest* m_iterator = robot->manifest;


	while(m_iterator != NULL){
		while(arrived_iterator != parkinglot->arrived_trucks){
			size  = truck_destination_robots_unloading_size(arrived_iterator);
			if(strncmp(m_iterator->package->destination, arrived_iterator->destination, MAX_DESTINATION_NAME_LEN) == 0 &&
					size < (arrived_iterator->capacity-arrived_iterator->size)){
				found_truck = 1;
				break;
			}

			arrived_iterator = arrived_iterator->next;
		}

		if(found_truck)
			break;
		m_iterator = m_iterator->next;
	}

	if(found_truck == 0)
		return 0;


	Robot* prevr_iterator = NULL;
	Robot* r_iterator = arrived_iterator->unloading_robots;
	while(r_iterator != NULL){
		Package *pkg = robot_get_destination_highest_priority_package(r_iterator, m_iterator->package->destination);
		if(m_iterator->package->priority >= pkg->priority)
			break;
		prevr_iterator = r_iterator;
		r_iterator = r_iterator->next;
	}

	robot->next = r_iterator;
	if(prevr_iterator == NULL)
		arrived_iterator->unloading_robots = robot;
	else
		prevr_iterator->next = robot;

	return 1;
}

void robot_print_manifest_info(Robot* robot){
	Manifest *iterator = robot->manifest;
	while(iterator != NULL){
		printf(" R->P: %s %ld\n", iterator->package->destination, iterator->package->priority);
		iterator = iterator->next;
	}

	printf("\n");
}



Truck* create_truck(const char* destination, long capacity, long transit_time, long departure_time){
	// TODO: 3.3

	Truck* t = (Truck*)malloc(sizeof(Truck));
	if(t == NULL) exit(1);

	t->manifest = NULL;
	t->unloading_robots = NULL;
	t->capacity = capacity;
	t->size = 0;
	
	if(destination != NULL){

		t->destination = strndup(destination, strlen(destination) + 1);
		//verific alocarea
		if(t->destination == NULL){
			free(t);
			exit(1);
		}
	}
	else
		t->destination = NULL;

	t->in_transit_time = 0;

	t->transit_end_time = transit_time;
	t->departure_time = departure_time;
	//fac legaturile
	t->next = NULL;
	return t;
}

int truck_is_full(Truck *truck){
	// TODO: 3.3
	//are nr maxim de elemente
	return (truck->size == truck->capacity);
}

int truck_is_empty(Truck *truck){
	// TODO: 3.3
	//nu are niciun element
	return (truck->size == 0);
}

long truck_destination_robots_unloading_size(Truck* truck){
	// TODO: 3.3
	//iterez
	Robot* r = truck->unloading_robots;
	//initializez numarul de pachete
	long nr = 0;

	while(r != NULL){
		//iterez prin manifest-ul robotului
		Manifest* m = r->manifest;

		while( m != NULL && m->package != NULL){

			//daca pachetul gasit area aceeasi destinatie cu cea dorita
			if(strcmp(m->package->destination, truck->destination) == 0){

				nr = nr + r->size; //creste numarul de pachete
				break;
			}
			//iterez
			if(m->next != NULL)
				m = m->next;
			else
				//daca nu mai am ce sa verific
				break;
		}
		//trec la robotul urmator
		if(r->next != NULL){
			r = r->next;	
		}
		//nu mai am alt robot
		else
			break;
	}

	return nr;
}


void truck_print_info(Truck* truck){
	printf("T: %s %ld %ld %ld %ld %ld\n", truck->destination, truck->size, truck->capacity,
			truck->in_transit_time, truck->transit_end_time, truck->departure_time);

	Manifest* m_iterator = truck->manifest;
	while(m_iterator != NULL){
		printf(" T->P: %s %ld\n", m_iterator->package->destination, m_iterator->package->priority);
		m_iterator = m_iterator->next;
	}

	Robot* r_iterator = truck->unloading_robots;
	while(r_iterator != NULL){
		printf(" T->R: %ld %ld\n", r_iterator->size, r_iterator->capacity);
		robot_print_manifest_info(r_iterator);
		r_iterator = r_iterator->next;
	}
}


void destroy_truck(Truck* truck){
	// TODO: 3.3

	Robot* r = truck->unloading_robots;
	//distrug toti robotii
	while(r != NULL){

		Robot* aux = r;
		r = r->next;
		destroy_robot(aux);
	}
	//distrug toate manifest-urile
	Manifest* m = truck->manifest;

	while(m != NULL){
		Manifest* m1 = m;
		m = m->next;
		destroy_manifest_node(m1);
	}
	//eliberez memoria
	free(truck->destination);
	free(truck);
}


Parkinglot* create_parkinglot(void){
	// TODO: 3.4
	// Allocate parking lot

	Parkinglot* p = (Parkinglot*)malloc(sizeof(Parkinglot));
	if(p == NULL) exit(1);

	p->arrived_trucks = create_truck(NULL, 0, 0, 0);
	if(p->arrived_trucks == NULL){
		free(p);
		exit(1);
	}

	p->departed_trucks = create_truck(NULL, 0, 0, 0);
	if(p->departed_trucks == NULL){
		free(p->arrived_trucks);
		free(p);
		exit(1);
	}

	p->pending_robots = create_robot(0);
	if(p->pending_robots == NULL){
		free(p->departed_trucks);
		free(p->arrived_trucks);
		free(p);
		exit(1);
	}

	p->standby_robots = create_robot(0);
	if(p->standby_robots == NULL){

		free(p->pending_robots);
		free(p->departed_trucks);
		free(p->arrived_trucks);
		free(p);
		exit(1);
	}
	//creez santinelele
	p->arrived_trucks->next = p->arrived_trucks;
	p->departed_trucks->next = p->departed_trucks;
	p->pending_robots->next = p->pending_robots;
	p->standby_robots->next = p->standby_robots;
	p->standby_robots = p->standby_robots->next;

	return p;
}

Parkinglot* open_parckinglot(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Parkinglot *parkinglot = create_parkinglot();

	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		// destination, capacitym transit_time, departure_time, arrived
		if(token[0] == 'T'){
			token = strtok(NULL, ",\n ");
			char *destination = token;

			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			token = strtok(NULL, ",\n ");
			long transit_time = atol(token);

			token = strtok(NULL, ",\n ");
			long departure_time = atol(token);

			token = strtok(NULL, ",\n ");
			int arrived = atoi(token);

			Truck *truck = create_truck(destination, capacity, transit_time, departure_time);

			if(arrived)
				truck_arrived(parkinglot, truck);
			else
				truck_departed(parkinglot, truck);

		}else if(token[0] == 'R'){
			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			Robot *robot = create_robot(capacity);
			parkinglot_add_robot(parkinglot, robot);

		}

		free(line);
		line = NULL;
		len = 0;
	}
	free(line);

	fclose(fp);
	return parkinglot;

	file_open_exception:
	return NULL;
}

void parkinglot_add_robot(Parkinglot* parkinglot, Robot *robot){
	// TODO: 3.4

	//pune in truck->stand_by_robots
	if(robot_is_empty(robot)){ 
		//lista e goala
		if(parkinglot->standby_robots->next == parkinglot->standby_robots){
			parkinglot->standby_robots->next = robot;
			robot->next = parkinglot->standby_robots;
			return;
		}
		//iterator
		Robot* r = parkinglot->standby_robots->next;

		//daca trebuie adaugat primul in lista
		if(robot->capacity > r->capacity){
			parkinglot->standby_robots->next = robot;
			robot->next = r;
			return;
		}
		//atata timp cat am elemente
		while(r->next != parkinglot->standby_robots){
			//il adaug la mijlog
			if( r->capacity > robot->capacity && r->next->capacity < robot->capacity){
				r->next = robot;
				robot->next = r->next;
				return;
			}
			//iterez
			r = r->next;
		}	
	}
	else{
		//pune in pending

		//lista e goala
		if(parkinglot->pending_robots->next == parkinglot->pending_robots){
			parkinglot->pending_robots->next = robot;
			robot->next = parkinglot->pending_robots;
			return;
		}
		//iterez pin roboti
		Robot* r1 = parkinglot->pending_robots->next;
		//daca trebuie pus primul in lista
		if(robot->size > r1->size){
			parkinglot->pending_robots->next = robot;
			robot->next = r1;
			return;
		}

		while(r1->next != parkinglot->pending_robots){
			//il adaug la miloc
			if(r1->size > robot->size && r1->next->size < robot->size){
				r1->next = robot;
				robot->next = r1->next;
				return;
			}
			r1 = r1->next;
		}
	}
}

void parkinglot_remove_robot(Parkinglot *parkinglot, Robot* robot){
	//TODO: 3.4
	//guard
	if(parkinglot == NULL || robot == NULL) return;

	//daca il scot din standby_robots
	if(robot_is_empty(robot)){ 
		//daca lista e goala, nu am ce sa scot
		if(parkinglot->standby_robots->next == parkinglot->standby_robots)
			return;
		//iteratori
		Robot* r = parkinglot->standby_robots->next;
		Robot* prev = parkinglot->standby_robots;
		//Robot* aux;

		while(r != parkinglot->standby_robots){
			if(r == robot){
				//daca il scot pe primul
				if(prev == parkinglot->standby_robots){
					//Robot* aux = r;
					parkinglot->standby_robots->next = r->next;
					//destroy_robot(aux);
					return;
				}
				//daca il cot pe ultimul
				if(r->next == parkinglot->standby_robots){
					//Robot* aux = r; 
					prev->next = parkinglot->standby_robots;
					//destroy_robot(aux);
					return;
				}
				//daca scot de la mijloc
				else{   
					//Robot* aux = r;             
					prev->next = r->next;
					r = r->next;
					//destroy_robot(aux);
					return;
				}
			}
			//iterez
			else{
				prev = r;
				r = r->next;
			}
		}

	}
	//daca scot din pending_robots
	else{

		if(parkinglot->pending_robots->next == parkinglot->pending_robots) return;

		Robot* r = parkinglot->pending_robots->next;
		Robot* prev = parkinglot->pending_robots;
		//Robot* aux;

		while(r != parkinglot->pending_robots){ 

			//aux = r;
			if(r == robot){
				//e primul
				if(prev == parkinglot->pending_robots){ 
					//Robot* aux = r;
					parkinglot->pending_robots->next = r->next;
					//destroy_robot(aux);
					return;
				}
				//e ultimul
				if(r->next == parkinglot->pending_robots){ 
					//Robot* aux = r;
					prev->next = parkinglot->pending_robots;
					//destroy_robot(aux);
					return;
				}
				//e la mijloc
				else{  
					//Robot* aux = r;                             
					prev->next = r->next;
					r = r->next;
					//destroy_robot(aux);
					return;
				}
			}
			//iterez
			else{
				prev = r;
				r = r->next;
			}
		}
	}
}


int parckinglot_are_robots_peding(Parkinglot* parkinglot){
	// TODO: 3.4
	//guard daca lista e goala
	if(parkinglot->pending_robots == parkinglot->pending_robots->next) return 0;
	//iterator
	Robot* r = parkinglot->pending_robots->next;
	//a trecut de guard, deci lista are cel putin un element
	int nr = 1;

	while(r->next != parkinglot->pending_robots){

		r = r->next;
		nr++;
	}

	return nr;
}

int parkinglot_are_arrived_trucks_empty(Parkinglot* parkinglot){
	// TODO: 3.4
	//lista este goala
	if(parkinglot->arrived_trucks->next == parkinglot->arrived_trucks) return 1;

	Truck* t = parkinglot->arrived_trucks->next;

	while(t != parkinglot->arrived_trucks){
		//daca truck-ul nu este gol, iese
		if(!truck_is_empty(t)) return 0;
		t = t->next;
	}
	//nu s-a gasit vreun tuck gol, deci returneaza 1
	return 1;
}


int parkinglot_are_trucks_in_transit(Parkinglot* parkinglot){
	// TODO: 3.4
	//intoarce 1 daca lista nu este goala
	return (parkinglot->departed_trucks->next != parkinglot->departed_trucks);
}


void destroy_parkinglot(Parkinglot* parkinglot){
	// TODO: 3.4

	//iterez
	Truck* t = parkinglot->arrived_trucks->next;
	//auxiliare pentru destroy
	Truck* aux;
	Robot* aux1;
	//distrug toate truck-urile care sunt arrived
	while(t != parkinglot->arrived_trucks){
		aux = t;
		t = t->next;
		destroy_truck(aux);
	}
	//distrug santinela
	destroy_truck(parkinglot->arrived_trucks);

	//schimb iteratorul
	t = parkinglot->departed_trucks->next;
	//distrug tot din departed trucks
	while(t != parkinglot->departed_trucks){
		aux = t;
		t = t->next;
		destroy_truck(aux);
	}
	//distrug iteratorul
	destroy_truck(parkinglot->departed_trucks);

	//parcurg robotii
	Robot* r =parkinglot->pending_robots->next;
	//distrug robotii care sunt in pending
	while(r != parkinglot->pending_robots){
		aux1 = r;
		r = r->next;
		destroy_robot(aux1);
	}
	//distrug santinela
	destroy_robot(parkinglot->pending_robots);

	//schimb iteratorul
	r = parkinglot->standby_robots->next;
	//distrug robotii din standby
	while(r != parkinglot->standby_robots){
		aux1 = r;
		r = r->next;
		destroy_robot(aux1);
	}
	//distrug santinela
	destroy_robot(parkinglot->standby_robots);

	//eliberez memoria pentru parcare
	free(parkinglot);
}

void parkinglot_print_arrived_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->arrived_trucks->next;
	while(iterator != parkinglot->arrived_trucks){

		truck_print_info(iterator);
		iterator = iterator->next;
	}

	printf("\n");

}

void parkinglot_print_departed_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->departed_trucks->next;
	while(iterator != parkinglot->departed_trucks){
		truck_print_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_pending_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->pending_robots->next;
	while(iterator != parkinglot->pending_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_standby_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->standby_robots->next;
	while(iterator != parkinglot->standby_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}


void truck_departed(Parkinglot *parkinglot, Truck* truck){
	// TODO: 3.5
	// Search through arrived list, if exists node is found remove it
	// Note: this must remove the node from the list, NOT deallocate it

	//guard
	if(parkinglot == NULL || truck == NULL) return;

	Truck* t = parkinglot->arrived_trucks->next;
	Truck* prev = parkinglot->arrived_trucks;
	Truck* aux ;
	while(t != parkinglot->arrived_trucks){

		aux = t; 
		// daca se gaseste in lista de arrived_trucks, il scoate
		if(t == truck){

			//il elimina pe primul din lista
			if(prev == parkinglot->arrived_trucks){
				parkinglot->arrived_trucks->next = t->next;
				//destroy_truck(aux);
				break;
			}
			//il elimina pe ultimul
			if(t->next == parkinglot->arrived_trucks){
				prev->next = parkinglot->arrived_trucks;
				//destroy_truck(aux);
				break;
			}
			//scoate de la mijloc
			else{
				prev->next = t->next;
				t = t->next;
				//destroy_truck(aux);

				break;
			}
		//itereaza prin arrived_trucks
		}
		else{
			prev = t;
			t = t->next;
		}
	}

	//adauga in departed trucks

	//daca lista e goala
	if(parkinglot->departed_trucks->next == parkinglot->departed_trucks){

		parkinglot->departed_trucks->next = truck;
		truck->next = parkinglot->departed_trucks;
		return;
	}
	//iteratori
	t = parkinglot->departed_trucks->next;
	prev = parkinglot->departed_trucks;

	while(t != parkinglot->departed_trucks){
		
		//daca trebuie sa l puna primul in lista
		if(prev == parkinglot->departed_trucks && truck->departure_time < t->departure_time){
			parkinglot->departed_trucks->next = truck;
			truck->next = t;
			return;
		}
		//daca il pune ultimul
		if(t->next == parkinglot->departed_trucks && truck->departure_time > t->departure_time){
			t->next = truck;
			truck->next = parkinglot->departed_trucks;
			return;
		}
		//il pune la mijloc
		if(truck->departure_time > prev->departure_time && truck->departure_time < t->departure_time){
			prev->next = truck;
			truck->next = t;
			return;
		}
		//itereaza
		prev = t;
		t = t->next;
	}
	destroy_truck(aux);
}

void truck_arrived(Parkinglot *parkinglot, Truck* truck){
	// TODO: 3.5
	// Search through departed list, if exists node is found remove it
	// Note: this must remove the node not deallocate it

	//guard
	if(parkinglot == NULL || truck == NULL) return;

	Truck* t = parkinglot->departed_trucks->next;
	Truck* prev = parkinglot->departed_trucks;

	//in scot din lista de departed
	while(t != parkinglot->departed_trucks){
	
		if(t == truck){
			
		 //il scoate din departed

		//daca scoate primul element din lista
		if(prev == parkinglot->departed_trucks){
			parkinglot->departed_trucks->next = t->next;
			break;
		}
		else
			//il scoate pe ultimul
			if(t->next == parkinglot->departed_trucks){
				t = prev;
				prev->next = parkinglot->departed_trucks;
				break;
			}
			//scoate de la mijloc
			else{
				prev->next = t->next;
				t = t->next;
				break;
			}

		}
		//itereaza
		else{
			prev = t;
			t = t->next;
		}
	}
	
	//dupa ge ajunge, goleste tirul

	//goleste manifest-urile
	while(truck->manifest != NULL){

		Manifest* aux = truck->manifest;

		truck->manifest = truck->manifest->next;

		destroy_manifest_node(aux);
	}


	//truck-ul este gol
	truck->size = 0;

	truck->in_transit_time = 0;

	//iteratori
	t = parkinglot->arrived_trucks->next;
	prev = parkinglot->arrived_trucks;

	//adaug in lista de arrived

	//adauga in lista goala
	if (t == parkinglot->arrived_trucks) {
		parkinglot->arrived_trucks->next = truck;

		truck->next = parkinglot->arrived_trucks;

		return;
	}
	
	while(t != parkinglot->arrived_trucks) {
		
		//compar destinatiile
		int x = strcmp(truck->destination, t->destination);
			
			//daca au aceeasi destinatie
			if (x == 0) {
				//le sortez dupa timp
				long t1 = truck->departure_time;
				long t2 = t->departure_time;

				if (t1 < t2) {
					//trebuie pus inaintea iteratorului t

					//il pun primul in lista
					if (prev == parkinglot->arrived_trucks) {
						truck->next = t;
						parkinglot->arrived_trucks->next = truck;
						return;
					}
					//il pun la mijloc, inaintea iteratorului t
					else {
						
						truck->next = t;
						prev->next = truck;
						return;
					}
				}
				//trebuie pus dupa iteratorul t
				else {
					//il pun la sfarsit
					if (t1 > t2 && t->next == parkinglot->arrived_trucks) {
						t->next = truck;
						truck->next = parkinglot->arrived_trucks;
						return;
					}
					//iterez
					else {
						
						prev = t;
						t = t->next;
					}
				}
			}
			else 
				//trebuie pus inaintea lui t
				if (x < 0) {
					//il pun primul in lista
					if (prev == parkinglot->arrived_trucks) {
						truck->next = t;
						parkinglot->arrived_trucks->next = truck;
						return;
					}
					//il pun la mijloc
					else {
					
						truck->next = t;
						prev->next = truck;
						return;
					}
				}
				//trebuie pus dupa t
				else {
					//il pun ultimul
					if (x > 0 && t->next == parkinglot->arrived_trucks) {
						t->next = truck;
						truck->next = parkinglot->arrived_trucks;
						return;
					}
					//iterez
					else {
						
						prev = t;
						t = t->next;
					}
				}
		}
	
}

void truck_transfer_unloading_robots(Parkinglot* parkinglot, Truck* truck){
	// TODO:  3.5.

	Robot*r = truck->unloading_robots;

	while(r != NULL){

		Robot* aux = r;
		r = r->next;
		//adauga robotul in parcare
	 	parkinglot_add_robot(parkinglot, aux);
	}
	//truck-ul nu mai are unloading robots 
	truck->unloading_robots = NULL;

	return;
}

// Depends on parking_turck_departed
void truck_update_depatures(Parkinglot* parkinglot, long day_hour){
	// TODO: 3.5

	Truck* t = parkinglot->arrived_trucks->next;

	while(t != parkinglot->arrived_trucks) {
		//daca are aceeasi ora de plecare
		if( t->departure_time == day_hour){
			//truck-u trece in departed
			Truck*aux = t;
			t = t->next;
			truck_departed(parkinglot, aux);
		}

		else t = t->next;
	}
	return;
}

// Depends on parking_turck_arrived
void truck_update_transit_times(Parkinglot* parkinglot){
 // TODO: 3.5

	Truck* t = parkinglot->departed_trucks->next;

	while(t != parkinglot->departed_trucks){

		Truck* aux = t;
		t = t->next;
		//actualizeaza timpul de tranzit
		aux->in_transit_time++;
		//daca s a terminat timpul
		if(aux->in_transit_time == aux->transit_end_time){
			//truck-ul trece in arrived
			aux->in_transit_time = 0;

			truck_arrived(parkinglot, aux);
		}
	}
}

void robot_swarm_collect(Wearhouse *wearhouse, Parkinglot *parkinglot){
	Robot *head_robot = parkinglot->standby_robots;
	Robot *current_robot = parkinglot->standby_robots->next;
	while(current_robot != parkinglot->standby_robots){

		// Load packages from wearhouse if possible
		if(!robot_load_packages(wearhouse, current_robot)){
			break;
		}

		// Remove robot from standby list
		Robot *aux = current_robot;
		head_robot->next = current_robot->next;
		current_robot = current_robot->next;

		// Add robot to the
		parkinglot_add_robot(parkinglot, aux);
	}
}


void robot_swarm_assign_to_trucks(Parkinglot *parkinglot){

	Robot *current_robot = parkinglot->pending_robots->next;

	while(current_robot != parkinglot->pending_robots){
		Robot* aux = current_robot;
		current_robot = current_robot->next;
		parkinglot_remove_robot(parkinglot, aux);
		int attach_succeded = robot_attach_find_truck(aux, parkinglot);
		if(!attach_succeded)
			parkinglot_add_robot(parkinglot, aux);
	}
}

void robot_swarm_deposit(Parkinglot* parkinglot){
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	while(arrived_iterator != parkinglot->arrived_trucks){
		Robot *current_robot = arrived_iterator->unloading_robots;
		while(current_robot != NULL){
			robot_unload_packages(arrived_iterator, current_robot);
			Robot *aux = current_robot;
			current_robot = current_robot->next;
			arrived_iterator->unloading_robots = current_robot;
			parkinglot_add_robot(parkinglot, aux);
		}
		arrived_iterator = arrived_iterator->next;
	}
}