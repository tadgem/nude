import shutil
import os
import argparse

CMAKE_VERSION_STR = "3.12"
CXX_VERSION_STR = "17"

def copy_and_overwrite(from_path, to_path):
    if os.path.exists(to_path):
        shutil.rmtree(to_path)
    shutil.copytree(from_path, to_path)


parser = argparse.ArgumentParser(description='Generate new names for template.')
parser.add_argument("--project_name", type=str, default="")
args = parser.parse_args()

template_lib_name = "library_name"
template_app_name = "app_template"
template_project_name = "project_name"
template_tests_name = "test_name"

library_name = args.project_name
app_name = library_name + "_app"
tests_name = library_name + "_tests"
project_name = library_name + "_project"

print("Project Name : ", project_name)
print("Library Name : ", library_name)

library_dir = "./" + library_name
tests_dir = "./" + library_name + "_tests"
app_dir = "./" + library_name + "_app"

def rename_files(root_dir):
    for root, dirs, files in os.walk(root_dir):
        path = root.split(os.sep)
        dir = os.path.basename(root)
        for file in files:
            final_path = dir + "/" + file
            try:
                filedata = str()
                with open(final_path, 'r') as file:
                    filedata = file.read()

                # Replace the target string
                filedata = filedata.replace(template_lib_name, library_name)
                filedata = filedata.replace(template_app_name, app_name)
                filedata = filedata.replace(template_tests_name, tests_name)

                # Write the file out again
                with open(final_path, 'w') as file:
                    file.write(filedata)

            except:
                pass

copy_and_overwrite("templates/library_name", library_dir)
copy_and_overwrite("templates/tests", "./" + tests_dir)
copy_and_overwrite("templates/app_template", app_dir)

os.rename(app_dir + "/app_template.cpp", app_dir + "/" + library_name + "_app.cpp")
os.rename(tests_dir + "/test_name.cpp", tests_dir + "/" + library_name + "_tests.cpp")
os.rename(library_dir + "/include/dummy.h", library_dir + "/include/" + library_name + ".h")
os.rename(library_dir + "/src/dummy.cpp", library_dir + "/src/" + library_name + ".cpp")

rename_files(library_dir)
rename_files(tests_dir)
rename_files(app_dir)

filedata = str()

open("CMakeLists.txt", 'w').close()

with open("CMakeLists.txt", 'r') as file:
    filedata = file.read()

filedata += "CMAKE_MINIMUM_REQUIRED(VERSION " + CMAKE_VERSION_STR + ")\n"
filedata += "project(" + project_name + ")\n"
filedata += "set(CMAKE_CXX_STANDARD " + CXX_VERSION_STR + ")\n"

filedata += "add_subdirectory(" + library_name + ")\n"
filedata += "add_subdirectory(" + app_name + ")\n"
filedata += "add_subdirectory(" + tests_name + ")\n"

# Write the file out again
with open("CMakeLists.txt", 'w') as file:
    file.write(filedata)