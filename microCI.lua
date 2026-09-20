config = {

	-- you can specify a alternative default docker image for each plugin
	-- this is useful when your company don't allow you to fetch images directly from the dockerhub
	default_docker_images = {
		-- Example:
		-- plugin_name = "artifactory.mycompany.com/some_folder/image_name:latest"
		bash = "",
		asciidoc = "intmain/microci_asciidoc:latest",
		docmd = "intmain/microci_docmd:0.3",
		doxygen = "",
		beamer = "pandoc/latex:latest",
		plantuml = "intmain/microci_plantuml:latest",
		pikchr = "intmain/microci_pikchr:latest",
		mermaid = "intmain/microci_mermaid:latest",
		git_deploy = "",
		git_publish = "bitnamilegacy/git:latest",
		mkdocs_material = "",
		cppcheck = "intmain/microci_cpp_compiler:latest",
		cpp = "intmain/microci_cpp_compiler:latest",
		clang_tidy = "intmain/microci_cpp_compiler:latest", -- clang-tidy
		clang_format = "intmain/microci_cpp_compiler:latest", -- clang-format
		vhdl_format = "intmain/microci_ghdl:latest",
		fetch = "bitnamilegacy/git:latest",
		minio = "minio/mc:latest",
		jfrog = "releases-docker.jfrog.io/jfrog/jfrog-cli-full-v2-jf",
		flawfinder = "intmain/microci_flawfinder:latest",
		docker_build = "",
		pandoc = "intmain/microci_pandoc:latest",
		template = "chevdor/tera:latest",
		raspberry_pico = "intmain/microci_raspberry_pico",
	},
}

-- AsciidocPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_asciidoc:latest");
-- BashPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step);
-- BeamerPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "pandoc/latex:latest");
-- ClangFormatPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_cpp_compiler:latest");
-- ClangTidyPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_cpp_compiler:latest");
-- CppCheckPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_cpp_compiler:latest");
-- CppPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_cpp_compiler:latest");
-- DocmdPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_docmd:0.3");
-- DoxygenPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step);
-- FetchPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "bitnamilegacy/git:latest");
-- FlawfinderPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_flawfinder:latest");
-- GitPublishPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "bitnamilegacy/git:latest");
-- JFrogPluginStepParser.cpp:  data["DOCKER_IMAGE"]      = stepDockerImage(step, "releases-docker.jfrog.io/jfrog/jfrog-cli-full-v2-jf");
-- MermaidPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_mermaid:latest");
-- MinioPluginStepParser.cpp:  data["DOCKER_IMAGE"]      = stepDockerImage(step, "minio/mc:latest");
-- PandocPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_pandoc:latest");
-- PikchrPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_pikchr:latest");
-- PlantumlPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_plantuml:latest");
-- PluginStepParser.cpp:auto PluginStepParser::stepDockerImage(const YAML::Node &step,
-- RaspberryPicoPluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_raspberry_pico");
-- SkipPluginStepParser.cpp:  data["DOCKER_IMAGE"]      = stepDockerImage(step);
-- TemplatePluginStepParser.cpp:  data["DOCKER_IMAGE"]     = stepDockerImage(step, "chevdor/tera:latest");
-- VHDLFormatPluginStepParser.cpp:  data["DOCKER_IMAGE"]  = stepDockerImage(step, "intmain/microci_ghdl:latest");
