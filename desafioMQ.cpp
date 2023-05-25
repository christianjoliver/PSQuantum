/*

	Projeto desenvolvido para vaga de estagio em Metodos 
	Quantitativos proposto pela empresa Quantum. 
	Este projeto consiste na implementacao de um modelo 
	de precificacao de titulos utilizando a biblioteca QuantLib.


	Project developed for an internship in Quantitative Methods 
	proposed by the company Quantum. This project consists of implementing 
	a security pricing model using the QuantLib library.

	Autor: Christian Jonas Oliveira

*/

#include <ql/qldefines.hpp>
#if !defined(BOOST_ALL_NO_LIB) && defined(BOOST_MSVC)
#  include <ql/auto_link.hpp>
#endif
#include <ql/pricingengines/bond/discountingbondengine.hpp>
#include <ql/termstructures/yield/bondhelpers.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace QuantLib;


// Definicao da estrutura que guardara os dados a serem manipulados no contexto
struct Titulo 
{
	int id, prazo;
    float vNominal;
    double tJuros;
	string nome;
};


/*
	Definicao: Funcao que aplica o modelo de Juros Fixos (Fixed Rate Bond Model)
	Entrada: (vector<Titulo>) vetor de titulos;
	Saida: (vector<Real>) vetor com valores calculados;
*/
vector<Real> FixedRateBondModel(vector<Titulo> vTitulos)
{
    vector<double> resultado;

    // Usando o calendario europeu
    Calendar calendar = TARGET();

    // Data de inicio
    Date dInicial(25, May, 2023);

    dInicial = calendar.adjust(dInicial);

    DayCounter cDias = ActualActual(ActualActual::ISDA);

    // Iterando para cada titulo do vetor
    for (auto ti : vTitulos)
    {

        // Data de vencimento
        Date dVencimento = calendar.advance(dInicial, Period(static_cast<Integer>(ti.prazo * 365), Days));

        InterestRate taxaJuros(ti.tJuros, cDias, Compounded, Annual);

        // Definicao da curva de desconto
        Handle<YieldTermStructure> tCurva( boost::make_shared<FlatForward>(dInicial, Handle<Quote>(boost::make_shared<SimpleQuote>(ti.tJuros)), cDias));

        // Definicao do cronograma
        Schedule cronograma(dInicial, dVencimento, Period(Annual), calendar, Following, Following, DateGeneration::Backward, false);

        // Definicao do objeto FixedRateBond
        vector<Rate>vtJuros(1, ti.tJuros * 1.0);
        FixedRateBond titulo(0, ti.vNominal, cronograma, vtJuros, cDias, ModifiedFollowing, ti.vNominal, dInicial);

        // Definicao do mecanismo de precificação
        boost::shared_ptr<PricingEngine> modelo(new DiscountingBondEngine(tCurva));
        titulo.setPricingEngine(modelo);

        // Armazenamento do resultado
        resultado.push_back(titulo.cleanPrice()); 
    }
    return resultado;
}


/*
	Definicao: Funcao que le o conteudo de um arq csv
	Entrada: (String) nome/caminho para um arquivo csv;
	Saida: Vetor de Objetos alvo;
*/
vector<Titulo> leArquivo(string arq) 
{
    // Abertura do arquivo
    ifstream arquivo(arq);
     
    vector<Titulo> vTitulos;

	if (arquivo.is_open())
    {
        string nome, linha;
        bool trigger = true;
        
        while (getline(arquivo, linha))
        {
            // Tratamento de primeira linha (nome das colunas da tabela)
            if(trigger)
            {
                trigger = false;
                continue;
            }

            vector<string> linha_dados;
            istringstream iss(linha);
            string valor;

            while (getline(iss, valor, ';')) 
            {
                linha_dados.push_back(valor);
            }
            
            Titulo t;
            t.id = stof(linha_dados[0]);
            t.nome = linha_dados[1];
            t.vNominal = stoi(linha_dados[2]);
            t.prazo = stoi(linha_dados[3]);
            t.tJuros = stod(linha_dados[4]);

            vTitulos.push_back(t);
        }
        arquivo.close();
	}
	return vTitulos;
}


/*
	Definicao: Funcao imprime os dados e resultados obtidos;
	Entrada: (vector<Titulo>) vetor de titulos, (vector<Real> resultado) 
            vetor contendo os resultados encontrados pelo metodo;
	Saida: void;
*/
void imprimeResultados(vector<Titulo> vTitulos, vector<Real> resultado)
{
    string symbol(50, '-');

    cout << symbol << std::endl;
    cout << "id\t" << "titulo\t" << "  valor_nominal\t" << "prazo\t" << "taxa_juros\t" << endl;
    for (Titulo v : vTitulos)
    {
        cout << v.id << "\t" << v.nome << "\t" << v.vNominal << "\t" << v.prazo << "\t" << v.tJuros << endl;
    }
    cout << symbol << std::endl;

    // Impressao dos dados

    for (auto i = 0; i < vTitulos.size(); i++)
    {
        cout << "Valor após o calculo do título " << vTitulos[i].nome << ": " << resultado[i] << endl;  
    }
    cout << symbol << std::endl;
}


int main() 
{    
    // Leitura do arquivo csv
    vector<Titulo> vTitulos = leArquivo("dados_titulos.csv");
    
    // Aplicacao do modelo implementado
    vector<Real> resultado = FixedRateBondModel(vTitulos);

    // Impressão dos resultados obtidos pelo modelo de precificacao de títulos de renda fixa
    imprimeResultados(vTitulos, resultado);

	return 0;
}
