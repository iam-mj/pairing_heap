#include <iostream>
#include <queue>
#include <vector>

using namespace std;

class PH;

//o clasa pentru nod
class Nod {
private:

    int valoare;
    //pentru fiecare nod vrem sa tinem minte un pointer catre copilul cel mai din stanga si catre fratele urmator
    Nod* copil;
    Nod* frateStanga; //un pointer suplimentar pentru a realiza increase-key
    Nod* frateDreapta;
    int rol; // 1 = primul copil, 2 = ultimul copil, 3 = singurul copil, 4 = un copil mijlociu, 0 = nimic - radacina =)

public:

    //constructori
    Nod()
    {
        this -> valoare = 0;
        this -> copil = nullptr;
        this -> frateStanga = nullptr;
        this -> frateDreapta = nullptr;
        this -> rol = 0;
    }
    Nod(int val)
    {
        this -> valoare = val;
        this -> copil = nullptr;
        this -> frateStanga = nullptr;
        this -> frateDreapta = nullptr;
        this -> rol = 0;
    }

    //metode
    void adaugaCopil(Nod* copilas)
    {
        if(this -> copil != nullptr) //daca are alti copii ne asiguram ca nu-i pierdem
        {
             //modificari nod copilas
             copilas -> frateDreapta = this -> copil;
             copilas -> frateStanga = this; //o sa fie parintele
             copilas -> rol = 1; //i setam rolul la un prim copil

             //modificam nodul care fusese primul pana acum
             //nodului care era inainte primul ii modificam fratele stang si rolul
             this -> copil -> frateStanga = copilas;
             //verificam daca la momentul asta este un nod singur sau nu ca sa stim cum ii modificam rolul
             if(this -> copil -> rol == 3) //daca era singur
                this -> copil -> rol = 2; //devine un ultim copil
             else {
                if(this -> copil -> rol == 1) //daca era doar primul ( <=> mai are frati)
                    this -> copil -> rol = 4; //devine un nod mijlociu
             }

             //modificam copilul nodului parinte
             this -> copil = copilas;
        }
        else //daca nu mai are copii
        {
            //actualizam copilul nodului parinte
            this -> copil = copilas;

            //modificam nodul copilas
            copilas -> frateStanga = this;
            copilas -> frateDreapta = this; //ambele voi fi parintele
            copilas -> rol = 3; //un singur copil
        }
    }

    friend class PH;
};

//o clasa pentru PairingHeap
class PH {
public:

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
        //daca nodul e ultimul => nu avem cu cn sa facem pereche => intoarcem un heap cu un singur nod, corespunzator codului curent
        if(nodCurent -> rol == 2 || nodCurent -> rol == 3)
        {
            PH p(nodCurent);
            return p;
        }
        else {
            //luam urmatoarele 2 noduri pe care le facem pereche
            Nod* nod1 = nodCurent;
            Nod* nod2 = nodCurent -> frateDreapta;
            Nod* nodUrmator;
            if(nod2 -> rol != 2) //ne asiguram ca mai avem un nod cu care sa continuam
                nodUrmator = nod2 -> frateDreapta;

            //tre sa stergem legaturile de frati dintre noduri
            nod1 -> frateDreapta = nullptr;
            nod2 -> frateDreapta = nullptr;

            //avem nevoie sa facem PH din noduri
            PH p1(nod1), p2(nod2);
            //realizam perechea reunindu-le
            PH pereche = reuniune(p1, p2);
            ///cout << "Perechea pt: " << nod1 -> valoare << " si " << nod2 -> valoare << endl;
            ///pereche.afisare();
            //ne asiguram ca nodUrmator nu e parintele
            PH heap;
            if(nod2 -> rol != 2)//3 nu poate fi fiindca avem nod1
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

    void increase_key(Nod* nod, int valDeAdunat)
    {
        //scoatem subheapul corespunzator nodului dat din heap
        //vedem ce fel de nod e
        //operatia e simpla, avem tot switch-ul asta ca sa ne asiguram ca rupem si refacem legaturile dintre noduri corect
        switch(nod -> rol)
        {
            case 1: //daca e un prim nod
                {
                    Nod* parinte = nod -> frateStanga;
                    parinte -> copil = nod -> frateDreapta; //il legam pe parinte de fratele din dreapta
                    nod -> frateDreapta -> frateStanga = parinte; //pe fratele din dreapta de parinte
                    //il facem pe fratele din dreapta prim nod sau singur nod
                    if(nod -> frateDreapta -> rol == 2) //daca era ultimul
                        nod -> frateDreapta -> rol = 3;
                    else nod -> frateDreapta -> rol = 1; //daca nu era ultimul => mai are frati dupa el

                    //rupem legaturile nodului nostru
                    nod -> frateStanga = nullptr;
                    nod -> frateDreapta = nullptr;
                    nod -> rol = 0;
                    break;
                }
            case 2: //daca e un ultim nod
                {
                    Nod* parinte = nod -> frateDreapta;
                    nod -> frateStanga -> frateDreapta = parinte; //legam penultimul frate de parinte
                    //si ii schimbam rolul
                    if(nod -> frateStanga -> rol == 1) //daca era primul acum a ramas singur
                        nod -> frateStanga -> rol = 3;
                    else nod -> frateStanga -> rol = 2; //daca nu era primul inseamna ca mai sunt frati inaintea lui

                    //rupem legaturile nodului nostru acum
                    nod -> frateStanga = nullptr;
                    nod -> frateDreapta = nullptr;
                    nod -> rol = 0;
                    break;
                }
            case 3: //daca e un nod singur
                {
                    nod -> frateStanga -> copil = nullptr; //frateStanga e parintele, care ramane fara copii
                    //rupem legaruile nodului nostru
                    nod -> frateStanga = nullptr;
                    nod -> frateDreapta = nullptr;
                    nod -> rol = 0;
                    break;
                }
            case 4: //daca e un nod la mijloc
                {
                    //ii legam fratele din stanga de cel din dreapta si invers, niciun rol nu se schimba
                    nod -> frateStanga -> frateDreapta = nod -> frateDreapta;
                    nod -> frateDreapta -> frateStanga = nod -> frateStanga;

                    //rupem legaturile nodului nostru
                    nod -> frateStanga = nullptr;
                    nod -> frateDreapta = nullptr;
                    nod -> rol = 0;
                    break;
                }
            default: //daca cumva deja e o radacina nu facem nimic
                {
                    break;
                }
        }

        //marim valoarea nodului
        nod -> valoare += valDeAdunat;

        //facem reuniunea dintre ph si subheap-ul care are nodul ca radacina
        PH subheap(nod); //un ph cu nodul nostru ca radacina
        *this = reuniune(*this, subheap);
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
            bool prima_parcurgere = true;
            if(next != nullptr)
            {
                do
                {
                    if(!prima_parcurgere)
                        next = next -> frateDreapta;
                    else prima_parcurgere = false;
                    //il trecem ca si copil al nodului curent
                    ierarhie[idx_curent].push_back(next -> valoare);
                    //verificam daca are copii, daca da, il adaugam in queue
                    if(next -> copil != nullptr)
                    {
                        noduri.push(make_pair(next, ++cnt));
                        ierarhie.push_back({});
                        ierarhie[cnt].push_back(next -> valoare);
                    }
                }
                while(next -> rol != 2 && next -> rol != 3); //pana ajungem la ultimul nod din lista de frati
            }
        }
        //afisam ierarhia
        for(int i = 0; i < ierarhie.size(); i++)
        {
            cout << "Copiii lui " << ierarhie[i][0] << " sunt: ";
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
    PH p, p2(12);
    p.inserare(3);
    p.inserare(4);
    p.inserare(5);
    Nod* pointer = p.Sradacina; //trageteaza 5-ul
    p.afisare();



    p2.inserare(10);
    p2.inserare(1);
    p2.afisare();

    p = reuniune(p, p2);
    p.afisare();

    p.inserare(6);
    p.inserare(7);
    p.afisare();

    p.increase_key(pointer, 2);
    p.afisare();

    p.stergere();
    p.afisare();

    p.stergere();
    p.afisare();
    return 0;
}

