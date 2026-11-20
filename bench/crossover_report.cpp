
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct Record {
    int n;
    long long m;
    double density;
    double primMedianMs;
    double kruskalMedianMs;
    double ratio;
};

std::vector<std::string> splitCSV(
    const std::string& line
) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;

    while (std::getline(stream, field, ','))
        fields.push_back(field);

    return fields;
}

std::string winner(double ratio) {
    if (ratio < 1.0) return "Prim";
    if (ratio > 1.0) return "Kruskal";
    return "Tie";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr
            << "Usage: crossover_report <results.csv>\n";
        return 1;
    }

    try {
        std::ifstream file(argv[1]);

        if (!file)
            throw std::runtime_error(
                "Cannot open results file"
            );

        std::string line;
        if (!std::getline(file, line))
            throw std::runtime_error("Missing CSV header");

        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line != "family,n,m,density,seed,repetitions,"
                    "prim_median_ms,kruskal_median_ms,ratio,mst_cost,status")
            throw std::runtime_error("Expected a crossover CSV with median runtimes");

        std::map<int, std::vector<Record>> groups;

        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (line.empty()) continue;

            auto fields = splitCSV(line);

            if (fields.size() != 11)
                throw std::runtime_error(
                    "Malformed CSV record"
                );

            if (fields[10] != "PASS")
                throw std::runtime_error(
                    "Unverified benchmark record"
                );

            // Analyze density sweeps separately.
            if (fields[0] != "random")
                continue;

            Record record{
                std::stoi(fields[1]),
                std::stoll(fields[2]),
                std::stod(fields[3]),
                std::stod(fields[6]),
                std::stod(fields[7]),
                std::stod(fields[8])
            };

            groups[record.n].push_back(record);
        }

        std::cout << std::fixed
                  << std::setprecision(4);

        std::cout
            << "PRIM VS KRUSKAL CROSSOVER ANALYSIS\n"
            << "=================================\n\n";

        for (auto& [n, records] : groups) {
            std::sort(
                records.begin(),
                records.end(),
                [](const Record& a, const Record& b) {
                    return a.density < b.density;
                }
            );

            std::cout << "Vertices: " << n << '\n';

            std::string previous = "";
            double previousDensity = 0;
            bool crossoverFound = false;

            for (const Record& r : records) {
                std::string current = winner(r.ratio);

                std::cout
                    << "  E=" << r.m
                    << " Density=" << r.density * 100 << "%"
                    << " Prim median=" << r.primMedianMs << "ms"
                    << " Kruskal median=" << r.kruskalMedianMs << "ms"
                    << " Ratio=" << r.ratio
                    << " Winner=" << current << '\n';

                if (!previous.empty() &&
                    previous != "Tie" &&
                    current != "Tie" &&
                    previous != current) {

                    crossoverFound = true;

                    std::cout
                        << "  Candidate crossover between "
                        << previousDensity * 100
                        << "% and "
                        << r.density * 100
                        << "% density\n";
                }

                if (current != "Tie") {
                    previous = current;
                    previousDensity = r.density;
                }
            }

            if (!crossoverFound) {
                std::cout
                    << "  No crossover observed "
                    << "at tested densities.\n";
            }

            std::cout << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: "
                  << e.what() << '\n';
        return 1;
    }

    return 0;
}
