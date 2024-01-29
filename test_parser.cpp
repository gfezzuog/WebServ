#include "test_parser.hpp"

FileReader::FileReader(const std::string& inputFileName) : inputFileName(inputFileName) {}

std::vector<std::string> FileReader::splitString(const std::string& input) {
    std::vector<std::string> result;
    std::string::size_type start;
    std::vector<std::string> dividedComponents;
	std::string::size_type end = 0;

	while (content.find('}', 0) != std::string::npos)
	{
		start = content.find('{', 0);
		end = content.find('}', 0);
		while (content[end - 1] != '\n')
			end = content.find('}', end + 1);
		if(end != std::string::npos)
			result.push_back(content.substr(start + 2, end - 2));
		content = content.substr(end + 2);
	}

    for (const auto& str : result) {
        std::stringstream ss(str);
        char delimiter = '{';

        std::string component;
        while (std::getline(ss, component, delimiter)) {
            if (!component.empty()) {
                dividedComponents.push_back(component);
            }
            delimiter = (delimiter == '{') ? '}' : '{';
        }
    }

    // Update the original vector with divided components
    result = dividedComponents;
    return (result);
}

void FileReader::printVector(const std::vector<std::string>& strings) {
    for (const auto& str : strings) {
        std::cout << str << std::endl;
        std::cout << std::endl << "----------------------------------------------------------------------------" << std::endl << std::endl;
    }
}

std::string FileReader::SaveContent() {
    std::ifstream inputFile(inputFileName);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << inputFileName << std::endl;
        //return;
    }

    char ch;
    while (inputFile.get(ch)) {
        content += ch;
    }

    inputFile.close();

    return (content);
}

int main() {
    std::string print;
    std::vector<std::string> lines;
    std::string inputFileName = "default_config_file.conf";   // Replace with your input file name

    FileReader fileReader(inputFileName);
    print = fileReader.SaveContent();
    lines = fileReader.splitString(print);
    fileReader.printVector(lines);

    return 0;
}

/*Vettore divide file conf in parti, l'idea é di far riconoscere poi ad un altra funzione come
salvare i dati presenti in questo vettore a seconda di come iniziano ES:

-inizio con / é il nome di un server e tutto ció che segue nel vettore successivo saranno le variabili
di esso che controlleremo e salveremo come con il parser vecchio

-se inizia con un numero sará l'inizio dei path e pure li li salveremo in modo simile

-necessario un controllo extra per parti del vettore che risulatno vuote che sono le parti finali di ogni
server, non l'ho controllato dal parser perché é piú facile aggiungere un if dopo piuttoato che un caso speciale
durante il parser

Per la divisione pensavo un vettore di classi (o una mappa se vogliamo) in cui la classe rappresenta ogni server e contiene
quindi le variabili e i metodi di get di ognuna.
*/