# microCI Help

The basic usage can be obtained by passing the `--help` option:

```
microCI --help



                           ░░░░░░░░░░░░░░░░░
                           ░░░░░░░█▀▀░▀█▀░░░
                           ░░░█░█░█░░░░█░░░░
                           ░░░█▀▀░▀▀▀░▀▀▀░░░
                           ░░░▀░░░░░░░░░░░░░
                           ░░░░░░░░░░░░░░░░░
                             microCI 0.47.0


Options:
  -h,--help                Print this help
  -V,--version             Print the microCI version
  -T,--test-config         Configuration test
  -A,--activity-diagram    Generate activity diagram
  -a,--append-log          Append log
  -O,--only name           Execute only a single step
  -l,--list                List steps
  -N,--number N[,N...]     Execute one or more numbered steps
  -x,--hash hh             Execute the hh step
  -U,--update-db           Update observability database
  -u,--update              Update microCI to stable stream
  -D,--update-dev          Update microCI to development stream
  -X,--uninstall           Uninstall
  -i,--input file.yml      Load the configuration from file.yml
  -H,--home alt_home_dir   Alternative home directory
  -c,--config gitlab_ci    Create a .gitlab-ci.yml example config
  -n,--new skip            Create a placeholder step
  -n,--new bash            Create a command line step
  -n,--new cpp             Create a C++ command line build step
  -n,--new docmd           Create a documentation step
  -n,--new asciidoc        Create a documentation step
  -n,--new mkdocs_material Create a documentation step
  -n,--new doxygen         Create a documentation step
  -n,--new pandoc          Create a document conversion step
  -n,--new git_publish     Create a publish step
  -n,--new git_deploy      Create a production deploy step
  -n,--new plantuml        Create a diagram generation step
  -n,--new pikchr          Create a diagram generation step
  -n,--new mermaid         Create a diagram generation step
  -n,--new clang-format    Create a code format step
  -n,--new vhdl-format     Create a code format step
  -n,--new beamer          Create a PDF presentation step
  -n,--new fetch           Create a download external artfact step
  -n,--new minio           Create a upload/download artifact step
  -n,--new jfrog           Create a upload/download artifact step
  -n,--new cppcheck        Create a C++ SAST step
  -n,--new clang-tidy      Create a C++ SAST step
  -n,--new flawfinder      Create a C++ SAST step
  -n,--new docker_build    Create a local docker build step
  -n,--new template        Create a template step
  -n,--new raspberry_pico  Create a embedded software build step - Raspberry Pico

```
