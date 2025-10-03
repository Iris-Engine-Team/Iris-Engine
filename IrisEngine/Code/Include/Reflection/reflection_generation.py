from pathlib import Path
import re
import sys
import os


def find_all_reflect_header():
    root_dir = Path(__file__).resolve().parent.parent
    header_files = []
    for dirpath, dirnames, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.endswith(".h"):
                full_path = os.path.abspath(os.path.join(dirpath, filename)).replace("\\", "/")
                with open(full_path, 'r', encoding='utf-8') as f:
                    file = f.read()
                if (file.find("REFLECT ") != -1):
                    header_files.append([full_path, file])
    
    return header_files
    
def clear_code(code):
    code = code.split('\n')
    suppr = False
    i = 0
    
    while i < len(code):
        if code[i] == '//automatically generated code':
            suppr = not suppr

            if (not suppr):
                del code[i]
                i -= 1

        if suppr:
            del code[i]
        else:
            i += 1
        
    return code


def get_relfected_type(code):

    reflected_type = [[]]
    line_index = 0
    nb_bracket = 0
    reflection = False

    while line_index < len(code):
        if reflection:
            reflected_type[-1].append(code[line_index])
            
            if code[line_index].find("{") != -1:
                nb_bracket += 1
                
            if code[line_index].find("}") != -1:
                nb_bracket -= 1

                if (nb_bracket == 0):
                    reflection = False
                    reflected_type[-1].append(line_index)
                    reflected_type.append([])

        elif code[line_index].find(" REFLECT ") != -1:
            reflection = True
            line_index -= 1

        line_index += 1
    
    return reflected_type


def reflect_type(reflection, reflected_enum, file_data):
    generated_code = []
    class_ids = []
    
    for i in range(len(reflection) - 1):
        if reflection[i][0].find('enum') != -1:
            reflected_enum.append(reflection[i])

        else:
            generated_code.append(generate_meta_code('\n'.join(reflection[i][:-1]), reflection[i][0].split(" ")[2]))
            class_ids.append(i)

    write_generated_code(file_data, [reflection[class_id][-1] for class_id in class_ids], generated_code)
            

def generate_meta_code(class_code, class_name):
    parents = None
    if (re.search(f'{class_name}\s*:\s*([^\n]+)', class_code)):
        parents = [re.sub(r'\b(public|protected|private)\b', '', parent).strip()for parent in re.search(f'{class_name}\s*:\s*([^\n]+)', class_code).group(1).split(',')]
    ctors = re.findall(r'IFUNCTION\s+{0}\s*\((.*?)\)'.format(class_name), class_code)
    functions = re.findall(r'IFUNCTION\s+(?:static\s+)?(?:[\w:<>]+)\s+(\w+)\s*\(', class_code)
    props = re.findall(r'IPROPERTY\s+(?:inline\s+static\s+)?[\w:<>]+\s+(\w+)', class_code)

    output = [
        '//automatically generated code',
        'private:',
        '    inline static std::hash<std::string_view> s_Hash{};',
        '',
        'public:',
        f'    inline static meta::factory<{class_name}> Factory = REFLECT_CLASS({class_name})'
    ]

    if (parents):
        for parent in parents:
            output.append(f'        .REFLECT_BASE({parent})');
    
    for ctor in ctors:
        default_val = True
        params = [p.strip().split()[0] for p in ctor.split(',') if p.strip()]
        
        while default_val:
            if params:
                output.append(f'        .REFLECT_CTOR({", ".join(params)})')
                if ctor.split(',')[-1].find('=') != -1:
                    ctor = ctor[:ctor.rfind(',')]
                    del params[-1]
                else:
                    default_val = False
            else:
                output.append('        .REFLECT_CTOR()')
                default_val = False;

    for func in functions:
        output.append(f'        .REFLECT_FUNC({func})')

    for prop in props:
        output.append(f'        .REFLECT_DATA({class_name}, {prop})')

    output[-1] += ';'
    output.append('//automatically generated code')

    return output


def write_generated_code(file_data, lines, generated_code):
    file = file_data[1]
    ln_index = 0

    for i in range(len(generated_code)):
        ln_index += lines[i];

        for line in generated_code[i]:
            file.insert(ln_index, line)
            ln_index += 1

        ln_index -= lines[i];

    with open(file_data[0], 'w', encoding='utf-8') as f:
        f.write("")
        f.write('\n'.join(file))

def reflect_enum(reflected_enum):
    reflection_file = [
    "#pragma once",
    "",
    '#include "Core/Tools/Reflection.h"',
    "",
    "namespace Iris",
    "{",
    "    ",
	"    namespace Engine",
	"    {",
    "        ",
	"        class EnumReflection",
	"        {",
	"            inline static std::hash<std::string_view> m_Hash{};",
    "            "]

    for enum in  reflected_enum:
        enum_name = enum[0].split(' ')[3];
        reflection_file.append(f'                inline static meta::factory<{enum_name}> m_{enum_name}Factory = REFLECT_ENUM({enum_name})')

        for line in enum[1:-1]:
            if line.find(',') != -1:
                elem_name = line.strip().split()[0]
                reflection_file.append(f'                    .REFLECT_ELEM({enum_name}::{elem_name}, {elem_name})')

    reflection_file += [
        "        };",
        "    }",
        "}"]

    file_path = os.path.abspath(Path(__file__).resolve().parent) + "\\EnumReflection.h"
    file_path.replace("\\", '/')
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write("")
        f.write('\n'.join(reflection_file))


def main():
    header_files = find_all_reflect_header()
    reflected_enum = []

    for file_data in header_files:
        file_data[1] = clear_code(file_data[1])
        reflection = get_relfected_type(file_data[1])

        if (reflection):
            reflect_type(reflection, reflected_enum, file_data)

        print("✅", file_data[0], "file generated.")

    reflect_enum(reflected_enum)
    print("✅ reflect enum file generated.")


if __name__ == "__main__":
    main()