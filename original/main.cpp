#include <iostream>
#include <fstream>
#include <queue>
#include <vector>

using namespace std;

ifstream f("mergeheap.in");
ofstream g("mergeheap.out");

class PH;

//o clasa pentru nod
class Nod {
private:

    int valoare;
    //pentru fiecare nod vrem sa tinem minte un pointer catre copilul cel mai din stanga si catre fratele urmator
    Nod* copil;
    Nod* frate;

public:

    //constructori
    Nod()
    {
        this -> valoare = 0;
        this -> copil = nullptr;
        this -> frate = nullptr;
    }
    Nod(int val)
    {
        this -> valoare = val;
        this -> copil = nullptr;
        this -> frate = nullptr;
    }

    //metode
    void adaugaCopil(Nod* copilas)
    {
        if(this -> copil != nullptr) //daca are alti copii ne asiguram ca nu-i pierdem
        {
             copilas -> frate = this -> copil;
             this -> copil = copilas;
        }
        else this -> copil = copilas;
    }

    friend class PH;
};

//o clasa pentru PairingHeap
class PH {
private:

    Nod* radacina;

public:

    //constructori
    PH() {
        this -> radacina = nullptr;
    }
    PH(int val)
    {
        this -> radacina = new Nod(val);
    }
    PH(Nod* radacina) {this -> radacina = radacina;}



    //metode
    bool empty() {return this -> radacina == nullptr;}

    int maxim() {
        return this -> radacina -> valoare;
    }

    void vidare() {
        this -> radacina = nullptr;
    }

    friend PH& reuniune(PH& heap1, PH& heap2);

    void inserare(int val)
    {
        PH aux = (*this);

        //construim un PH care are doar un nod cu valoarea data
        PH phNou(val);

        //reunim pairing heap-ul tocmai construit cu *this
        *this = reuniune(aux, phNou);
    }

    //metoda specifica pentru a sterge radacina
    PH twoPassMerge(Nod* nodCurent)
    {
        //verificam daca mai avem noduri sa continuam sau ne oprim
        //daca nodul curent e null, intoarcem un heap vid
        if(nodCurent == nullptr)
        {
            PH p;
            return p;
        }
        //daca fratele e null => nu avem cu cn sa facem pereche => intoarcem un heap cu un singur nod, corespunzator codului curent
        if(nodCurent -> frate == nullptr)
        {
            PH p(nodCurent);
            return p;
        }
        else {
            //luam urmatoarele 2 noduri pe care le facem pereche
            Nod* nod1 = nodCurent;
            Nod* nod2 = nodCurent -> frate;
            Nod* nodUrmator = nod2 -> frate;

            //tre sa stergem legaturile de frati dintre noduri
            nod1 -> frate = nullptr;
            nod2 -> frate = nullptr;

            //avem nevoie sa facem PH din noduri
            PH p1(nod1), p2(nod2);
            //realizam perechea reunindu-le
            PH pereche = reuniune(p1, p2);
            ///cout << "Perechea pt: " << nod1 -> valoare << " si " << nod2 -> valoare << endl;
            ///pereche.afisare();
            //ne asiguram ca nodUrmator nu e null
            PH heap;
            if(nodUrmator != nullptr)
            {
                //unim perechea cu urmatoarele perechi
                PH urmatoarePerechi = twoPassMerge(nodUrmator);
                heap = reuniune(pereche, urmatoarePerechi);
            }
            else heap = pereche;

            return heap;
        }
    }

    void stergere()
    {
        ///cout << "Stergem " << this -> radacina -> valoare << endl;
        Nod* aux = this -> radacina;

        Nod* inceput = this -> radacina -> copil;
        *this = twoPassMerge(inceput); //this -> implicit

        delete aux; //ne asiguram ca stergem nodul radacina
    }

    void afisare()
    {
        vector <vector <int>> ierarhie; //pentru fiecare nod tinem minte ce copii are
        //parcurgem heap-ul
        queue <pair <Nod*, int>> noduri;
        int cnt = 0;
        noduri.push(make_pair(this -> radacina, cnt));
        ierarhie.push_back({});
        ierarhie[cnt].push_back((this -> radacina) -> valoare);
        //in ierarhie pt fiecare nod ii scriem mai intai valoarea lui, apoi copii
        while(!noduri.empty())
        {
            //pt fiecare nod ii parcurgem copii, ii trecem in lista lui de copii si ii adaugam in queue daca au si ei la randul lor copii
            Nod* curr = noduri.front().first;
            int idx_curent = noduri.front().second;
            noduri.pop();
            Nod* next = curr -> copil;
            while(next != nullptr)
            {
                //il trecem ca si copil al nodului curent
                ierarhie[idx_curent].push_back(next -> valoare);
                //verificam daca are copii, daca da, il adaugam in queue
                if(next -> copil != nullptr)
                {
                    noduri.push(make_pair(next, ++cnt));
                    ierarhie.push_back({});
                    ierarhie[cnt].push_back(next -> valoare);
                }
                next = next -> frate;
            }
        }
        //afisam ierarhia
        for(int i = 0; i < ierarhie.size(); i++)
        {
            cout << "Copii lui " << ierarhie[i][0] << " sunt: ";
            for(int j = 1; j < ierarhie[i].size(); j++)
                cout << ierarhie[i][j] << ' ';
            cout << endl;
        }
        cout << endl << endl;

    }
};
PH& reuniune(PH& heap1, PH& heap2)
{
    //verificam daca vreun heap este gol
    if(heap1.empty())
        return heap2;
    if(heap2.empty())
        return heap1;


    if(heap1.maxim() > heap2.maxim())
    //daca primul heap are radacina mai mare
    {
        //adaugam radacina lui heap2 ca si copil al radacinii lui heap1
        heap1.radacina -> adaugaCopil(heap2.radacina);
        return heap1;
    }
    else {
        //facem acelasi lucru pentru heap2
        heap2.radacina -> adaugaCopil(heap1.radacina);
        return heap2;
    }
}

int main()
{
    //rezolvare mergeHeap

    int n, q; //n - numarul de pairingHeaps, q - numarul de queries
    f >> n >> q;
    ///cin >> n >> q;
    vector <PH> heaps; //tinem un vector de heapuri
    for(int i = 0; i < n; i++)
    {
        PH p; //cream un obiect generic
        heaps.push_back(p);
        //nu e pb ca se sterge p fiindca push_back imi patreaza o copie
    }
    int optiune;
    for(int i = 1; i <= q; i++)
    {
        f >> optiune;
        ///cin >> optiune;
        switch(optiune)
        {
        case 1:
            {
                //citim in ce multime inseram un element x
                int index, x;
                f >> index >> x;
                ///cin >> index >> x;
                //realizam inserarea
                heaps[index].inserare(x);
                break;
            }
        case 2:
            {
                //citim din ce multime afisam maximul, pe care ulterior il stergem
                int index;
                f >> index;
                ///cin >> index;
                //afisam maximul
                g << heaps[index].maxim() << '\n';
                ///cout << heaps[index].maxim() << '\n';
                //stergem maximul
                heaps[index].stergere();
                break;
            }
        case 3:
            {
                //citim multimile a si b pe care le reunim
                int a, b;
                f >> a >> b;
                ///cin >> a >> b;
                heaps[a] = reuniune(heaps[a], heaps[b]);
                //vidam multimea b
                heaps[b].vidare();
                break;
            }
        }
    }
    f.close();
    g.close();
    return 0;
}
