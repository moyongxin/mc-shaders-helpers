# generate template.lang file from shaders.properties in the same directory
# only generate profile, screen and option entries

import os


def read_file(filename):
    if not os.path.exists(filename):
        raise FileNotFoundError(f"File not found: {filename}")
    with open(filename, "r") as f:
        return f.read()


def write_file(filename, content):
    with open(filename, "w") as f:
        f.write(content)


input_file = read_file("shaders.properties")
input_file = input_file.split("\\")
input_file = "".join([line.strip("\n\t ") + " " for line in input_file])
input_file = input_file.split("\n")
input_file = [line.strip("\t\n ") for line in input_file if line.strip("\t\n ") != ""]

filtered_lines = [line for line in input_file if not line.startswith("#")]

profiles = []
screens = []
reserved_names = ["<empty>", "<profile>"]
options = []

for line in filtered_lines:
    if not line.strip():
        continue

    parsed_line = line.split("=", 1)
    parsed_line = [line.strip() for line in parsed_line]
    key = parsed_line[0].strip()
    values = [value.strip() for value in parsed_line[1].split(" ") if value != ""]
    key = [i for i in key.split(".") if key != ""]

    if key[0] == "profile":
        profiles.append(key[1])

    if key[0] == "screen":
        if key[-1] == "columns":
            continue

        tmp = [i for i in values if not i in reserved_names]
        for i in tmp:
            if i.startswith("[") and i.endswith("]"):
                screens.append(i[1:-1])
            else:
                options.append(i)

options = list(set(options))
screens = list(set(screens))

profiles.sort()
options.sort()
screens.sort()

result = []
result.append("# AUTOGENERATED FILE")
result.append("# GENERATED BY template_lang.py script authored by qwertyuiop")
result.append("\n# PROFILES")
for profile in profiles:
    result.append(f"profile.{profile}={profile}")

result.append("\n# SCREENS")
for screen in screens:
    result.append(f"screen.{screen}={screen}")

result.append("\n# OPTIONS")
comments = []
for option in options:
    result.append(f"option.{option}={option}")
    comments.append(f"option.{option}.comment={option}")
result.append("\n# OPTION COMMENTS")
result.extend(comments)

write_file("template.lang", "\n".join(result))
