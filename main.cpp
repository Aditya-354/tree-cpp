#include <iostream>
#include <vector>
#include <variant>
#include <string_view>
#include <string>
#include <span>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

struct PrintOptions {
    std::optional<size_t> max_depth {};
    fs::path dir_to_print { fs::current_path() };
};

// │
//
// └
//
// ─
//
// ├

static constexpr std::string_view PATH_ARGUMENT_BEGIN { "--path=" };

PrintOptions create_print_options(std::span<char*> arguments) {
    PrintOptions print_options;
    size_t i { 1 };
    while(i < arguments.size()) {
        std::string argument { arguments[i] };
        i++;
        
        if(argument == "-L") {
            if(arguments.size() == i) {
                std::cerr << "Wrong usecase of -L. Number argument required after -L.\n";
                continue;
            }

            std::string number_argument { arguments[i] };
            i++;

            try {
                print_options.max_depth = std::stoul(number_argument);
            } 
            catch(...) {
                std::cerr << "Wrong usecase of -L. An integer should be placed after -L.\n";
                continue;
            }
        } 
        else if(argument.starts_with(PATH_ARGUMENT_BEGIN)) {
            std::filesystem::path usr_path { argument.substr(PATH_ARGUMENT_BEGIN.size()) };
            if(!fs::exists(usr_path)) {
                std::cerr << "please provide a valid path after '--path='\n";
                continue;
            }
            print_options.dir_to_print = usr_path;
        }
    }
    return print_options;
}


struct File {
    fs::path path;
};

struct Directory {
    fs::path path;
};

struct Root {
    fs::path path;
};

using PrintableFileObject = std::variant<File, Directory, Root>;
int size_ {};

class FileObjectPrinter {
    private:
        std::vector<bool> m_stack {};
        PrintOptions m_options {};

        void print_prefix(bool is_last) {
            for(bool has_more_siblings : m_stack) {
                if(has_more_siblings) {
                    std::cout << "│   ";
                }
                else {
                    std::cout << "    ";
                }
            }
            std::cout << (is_last ? "└── " : "├── ");
        }

        void print_children(const fs::path& path) {
            if(m_options.max_depth.has_value() && m_stack.size() >= *m_options.max_depth)
                return;
            std::vector<fs::path> children {};
            for(const fs::directory_entry& child : fs::directory_iterator(path)) {
                children.push_back(child);
            }

            for(size_t i {}; i < children.size(); i++) {
                bool is_last { i + 1 == children.size() };
                print_prefix(is_last);
                m_stack.push_back(!is_last);
                std::visit(*this, create_printable(children[i]));
                m_stack.pop_back();
            }
        }

        PrintableFileObject create_printable(const fs::path& path) {
            if(fs::is_directory(path))
                return Directory { path };
            return File { path };
        }
    public:
        FileObjectPrinter(const PrintOptions& print_options)
            : m_options(print_options) 
        {}

        void print() {
            std::visit(*this, PrintableFileObject { Root { m_options.dir_to_print } });
        }

        void operator()(const File& file) {
            if(exceeds_depth())
                return;
            std::cout << file.path.filename().string() << "\n";
        }

        void operator()(const Directory& directory) {
            if(exceeds_depth())
                return;
            std::cout << directory.path.filename().string() << "\n";
            print_children(directory.path);
        }

        void operator()(const Root& root) {
            std::cout << ".\n";
            print_children(root.path);
        }

        bool exceeds_depth() {
            return m_options.max_depth.has_value() && m_stack.size() > *m_options.max_depth;
        }
};

int main(int argc, char** argv) {
    PrintOptions print_options { create_print_options(std::span<char*>(argv, argc)) };

    std::cout << "---OPTIONS---\n";
    std::cout << "dir to print: " << print_options.dir_to_print.filename().string() << '\n';
    std::cout << "maximum depth: " << (
            print_options.max_depth.has_value() ?
            std::to_string(*print_options.max_depth) 
            : "infinite"
            ) << '\n';

    // std::visit(FileObjectPrinter { print_options }, PrintableFileObject { Root { print_options.dir_to_print } });

    FileObjectPrinter printer { print_options };
    printer.print();
    return 0;
}
