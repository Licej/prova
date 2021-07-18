#include <iostream>
#include "conto.h"
#include "grafo.h"
#include <fstream>
#include <cstring>

using namespace std;

/**
 * @brief Funzione che stampa tutti i nodi del grafo con i relativi nomi e budget.
 */

/**
 * La funzione stampa il nome che corrisponde all'indice corrente del loop e il suo budget,
 * poi utilizzando una varibile temporaneo scorre tutti i suoi archi uscenti e ne stampa i nomi
 * e il peso che corrisponde al valore del nodo. Per farlo utilizza le funzioni ::get_dim, ::print,
 * ::get_adjlist, ::get_adjnode, ::get_adjweight, ::get_nextadj.
 * @param g: grafo
 * @param vett: vettore dinamico con i nomi e i budget
 * @return Niente.
 */
void stampa(graph g,tipo_inf *vett){
	for(int i=0;i<get_dim(g);i++){
		print(vett[i]);
		cout<<", Pagamenti: ";
		adj_list tmp=get_adjlist(g,i+1);
		while(tmp!=NULL){
			cout<<vett[get_adjnode(tmp)-1].nome
				<<"("<<get_adjweight(tmp)<<")";
			tmp=get_nextadj(tmp);
			if(tmp!=NULL)
				cout<<", ";
		}
		cout<<endl;
	}
}
/**
 * @brief Funzione che aggiorna il peso corrispondente ai due nomi in ingresso o che crea l'arco.
 */

/**
 * La funzione cerca i due indice corrispondenti ai nomi passati come parametri. Nel caso non li trovi stampa un messaggio di
 * errore altrimenti cerca negli archi della lista di adiacenza che corrispondono al primo nome. Se trova l'indice che corrisponde
 * al secondo nome, aggiorna il peso aggiungendo quello passato alla funzione, altrimenti crea l'arco corrispondente.
 * @param g: il grafo
 * @param nodes: il vettore dinamico contente gli utenti
 * @param nome1: il primo nome da cui cercare gli archi uscenti
 * @param nome2: il secondo nome di cui viene aggiornato l'arco entrante o viene creato
 * @param w: il peso(cioe' i soldi) da aggiungere
 * @return Nulla
 */

void update(graph g, tipo_inf* nodes, char* nome1, char* nome2, int w){
	int indice1=-1;
	int indice2=-1;
	for(int i=0;i<get_dim(g) && (indice1==-1 || indice2==-1);i++){
		cout<<nodes[i].nome<<" ";
		if(strcmp(nodes[i].nome,nome1)==0)
			indice1=i;
		else if(strcmp(nodes[i].nome,nome2)==0)
			indice2=i;
	}
	cout<<endl;
	if(indice1==-1 || indice2==-1){
		cout<<"Hai inserito un nome sbagliato!"<<endl;
		return;
	}
	bool trovato=false;
	adj_list tmp=get_adjlist(g,indice1+1);
	while(tmp!=NULL && !trovato){
		if(get_adjnode(tmp)-1==indice2){
			tmp->weight+=w;
			trovato=true;
		}
		else
			tmp=get_nextadj(tmp);
	}
	if(!trovato)
		add_arc(g,indice1+1,indice2+1,w);
}
/**
 * @brief Funzione che se trova tra gli archi uscente della lista di adiacenza che corrisponde al nome passato, li semplifica
 */

/**
 * La funzione inizia trovando l'indice che corrisponde al nome passato alla funzione e se non lo trova stampa un messaggio di errore.
 * In caso contrario prende la lista di dell'indice trovato e la scorre, tenenendo conto ogni volta che si passa all'elemento successivo
 * della lista, anche dell'elemento precedente. A questo punto viene presa lista di adiacenza che corrisponde al valore del nodo
 * della lista di partenza, e viene scorsa cercando un arco con lo stesso peso dell'arco da cui è stata presa la seconda lista di adiacenza.
 * Nel caso venga trovato, la funzione cerca nella prima lista di adiacenza se un arco con lo stesso valore è già presente e nel
 * caso somma al peso del suo arco, il peso trovato prima, altrimenti crea un nuovo arco. Successivamente elimina l'arco dalla secondo lista di
 * di adiacenza e anche quello di partenza dalla prima.
 * @param g: grafo
 * @param vett: vettore dinamico degli utenti
 * @param nome: nome dell'utente da cui parte la ricerca di semplificazione
 * @return Nulla
 */
void semplifica(graph g,tipo_inf *vett, const char* nome){
	int indice=-1;
	for(int i=0;i<get_dim(g) && indice==-1;i++){
		if(strcmp(vett[i].nome,nome)==0)
			indice=i;
	}
	if(indice==-1){
		cout<<"Il nome inserito non è corretto!"<<endl;
		return;
	}

	adj_list sorgente=get_adjlist(g,indice+1);
	adj_list precedente=NULL;
	while(sorgente!=NULL){
		adj_list esplora=get_adjlist(g,get_adjnode(sorgente));
		adj_node *prev=NULL;
		bool scambio=false;
		while(esplora!=NULL && !scambio){
			if(get_adjweight(sorgente)==get_adjweight(esplora) && indice!=get_adjnode(esplora)-1){
				int nodo=get_adjnode(esplora);
				int peso=get_adjweight(esplora);
				bool trovato=false;
				adj_list controllo=get_adjlist(g,indice+1);
				while(controllo!=NULL && !trovato){
					if(get_adjnode(esplora)==get_adjnode(controllo)){
						controllo->weight+=get_adjweight(esplora);
						trovato=true;
					}
					controllo=get_nextadj(controllo);
				}
				if(prev!=NULL){
					prev->next=get_nextadj(esplora);
					delete [] esplora;
				}
				else{
					g.nodes[get_adjnode(sorgente)-1]=get_nextadj(g.nodes[get_adjnode(sorgente)-1]);
					delete [] esplora;
					esplora=get_adjlist(g,get_adjnode(sorgente));
				}
				adj_list elimina=sorgente;
				if(precedente!=NULL)
					precedente->next=get_nextadj(sorgente);
				else
					g.nodes[indice]=get_nextadj(g.nodes[indice]);
				sorgente=get_nextadj(sorgente);
				delete [] elimina;
				scambio=true;
				if(!trovato){
					add_arc(g,indice+1,nodo,peso);
				}
			}
			else{
				prev=esplora;
				esplora=get_nextadj(esplora);
			}
		}
		if(!scambio){
			precedente=sorgente;
			sorgente=get_nextadj(sorgente);
		}
	}
}

bool controllo(graph g, tipo_inf *vett){
	int *soldi_totali=new int[get_dim(g)];
	for(int i=0;i<get_dim(g);i++)
		soldi_totali[i]=0;
	for(int i=0;i<get_dim(g);i++){
		soldi_totali[i]+=vett[i].budget;
		adj_list tmp=get_adjlist(g,i+1);
		while(tmp!=NULL){
			soldi_totali[get_adjnode(tmp)-1]+=get_adjweight(tmp);
			tmp=get_nextadj(tmp);
		}
	}
	for(int i=0;i<get_dim(g);i++){
		int spese=0;
		adj_list tmp=get_adjlist(g,i+1);
		while(tmp!=NULL){
			spese+=get_adjweight(tmp);
			tmp=get_nextadj(tmp);
		}
		if(soldi_totali[i]-spese<0){
			delete [] soldi_totali;
			return false;
		}
	}
	delete [] soldi_totali;
	return true;
}

int main(){
	ifstream f("soldi.txt");
	int utenti,archi;
	f>>utenti>>archi;
	graph g=new_graph(utenti);
	tipo_inf *vett=new tipo_inf[utenti];
	for(int i=0;i<utenti;i++)
		f>>vett[i].nome>>vett[i].budget;
	for(int i=0;i<archi;i++){
		int u,v,weight;
		f>>u>>v>>weight;
		add_arc(g,u,v,weight);
	}
	stampa(g,vett);
	char nome1[40],nome2[40];
	int peso;
	cout<<"Inserisci i nomi e il peso: ";
	cin>>nome1>>nome2>>peso;
	update(g,vett,nome1,nome2,peso);
	stampa(g,vett);
	semplifica(g,vett,"Mario");
	stampa(g,vett);
	if(controllo(g,vett))
		cout<<"Tutti i pagamenti sono ammissibili"<<endl;
	else
		cout<<"Alcuni pagamenti superano il budget"<<endl;
	if(vett!=NULL)
		delete [] vett;
	return 0;
}
