import xml.etree.ElementTree as ET


tree = ET.parse('gsgs.xml')
 
root = tree.getroot()

print(root[0].attrib['Count'])
for child in root[0]:
    print(child.attrib['Index'], child.attrib['Name'])

print("")

print(root[1].attrib['Count'])
for child in root[1]:
    print(child.attrib['Index'], child.attrib['SymbolCount'],child.attrib['NonTerminalIndex'])

print("")

print(root[2].attrib['Count'])
for child in root[2]:
    print(child.attrib['Index'], len(child))
    for sub in child:
        print(sub.attrib['SymbolIndex'], sub.attrib['Action'], sub.attrib['Value'])

