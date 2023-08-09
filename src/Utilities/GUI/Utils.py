import re
from IPython import embed
def extract_enum_elements(header_file, enum_name):
    with open(header_file, 'r') as file:
        content = file.read()
        if enum_name == 'DetectorType':
            pat = 'enum'
        elif enum_name == 'InjectionMode':
            pat = 'enum class'
        else:
            print("Wrong enum_name (%s)" %enum_name)
            return []
        # find enum expression using regular expressions (re)
        pattern = r'%s\s+(\w+)\s*{([^}]*)}' %pat
        matches = re.findall(pattern, content, re.MULTILINE | re.DOTALL)

        enum_dict = {}
        for match in matches:
            # enum expression found in file
            if match[0] == enum_name:
                enum_elements = match[1]

                # extract enum elements using re
                element_pattern = r'(\w+)\s*=?\s*[^,\n]*'
                elements = re.findall(element_pattern, enum_elements)

                if 'eUnknown' in elements:
                    elements.remove('eUnknown')
                return elements

    return []
