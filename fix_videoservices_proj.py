import xml.etree.ElementTree as ET

# Read the vcxproj file
with open(r'c:\originalhr\HoverRace\NetTarget\VideoServices\VideoServices.vcxproj', 'r') as f:
    content = f.read()

# Parse XML
root = ET.fromstring(content)

# Define namespace
ns = {'msbuild': 'http://schemas.microsoft.com/developer/msbuild/2003'}

# Find all Link elements in ItemDefinitionGroups and fix dependencies
count_links = 0
for itemdef in root.findall('.//msbuild:ItemDefinitionGroup', ns):
    link = itemdef.find('.//msbuild:Link', ns)
    if link is not None:
        dep = link.find('msbuild:AdditionalDependencies', ns)
        if dep is not None:
            # Get current text
            curr_text = dep.text or ""
            # Remove \gdk\sdk references - just keep modern libs
            new_val = "dsound.lib;winmm.lib;%(AdditionalDependencies)"
            dep.text = new_val
            count_links += 1
            print(f"Fixed Link dependencies: {new_val}")

# Remove legacy include directories
count_includes = 0
for compile_group in root.findall('.//msbuild:ClCompile', ns):
    includes = compile_group.find('msbuild:AdditionalIncludeDirectories', ns)
    if includes is not None:
        text = includes.text or ""
        # Remove \gdk\sdk\inc; if present
        if r'\gdk\sdk\inc' in text:
            parts = text.split(';')
            parts = [p for p in parts if r'\gdk\sdk\inc' not in p]
            new_text = ';'.join(parts)
            includes.text = new_text
            count_includes += 1
            print(f"Cleaned includes: {new_text[:80] if new_text else '(removed all)'}")

# Pretty print and save
ET.indent(root, space="  ")
output = ET.tostring(root, encoding='unicode')

with open(r'c:\originalhr\HoverRace\NetTarget\VideoServices\VideoServices.vcxproj', 'w') as f:
    f.write('<?xml version=\'1.0\' encoding=\'utf-8\'?>\n')
    f.write(output)

print(f"\nFixed {count_links} Link sections and {count_includes} Include sections")
print("VideoServices.vcxproj updated!")
