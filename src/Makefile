
CXXFLAGS+= -DFMT_HEADER_ONLY
CXXFLAGS+= -DSPDLOG_FMT_EXTERNAL=1

CXXFLAGS+= -I../include/

CXXFLAGS+= -s
CXXFLAGS+= -O3
CXXFLAGS+= -static
CXXFLAGS+= -fPIC
CXXFLAGS+= -std=c++17
LDFLAGS+= -lstdc++fs
LDFLAGS+= -lyaml-cpp

all: deps microCI test

test:
	./microCI -i y.yml      || echo "OK"
	./microCI --input=x.yml || echo "OK"
	./microCI -h > help.txt
	./microCI > default.sh
	./microCI -i git_deploy.yml > git_deploy.sh
	./microCI -i mkdocs_material.yml > mkdocs_material.sh
	./microCI -i mkdocs_material.yml -O "local-step" > mkdocs_material_only.sh


microCI: MicroCI.cpp main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	strip --strip-unneeded $@

deps: ../include/argh.hpp ../include/inja.hpp ../include/nlohmann/json.hpp

../include/argh.hpp:
	curl https://raw.githubusercontent.com/adishavit/argh/master/argh.h -o $@

../include/inja.hpp:
	curl https://raw.githubusercontent.com/pantor/inja/master/single_include/inja/inja.hpp -o $@

../include/nlohmann/json.hpp:
	curl https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp -o $@

clean:
	rm -f microCI
	rm -f ../include/argh.hpp
	rm -f ../include/inja.hpp
	rm -f ../include/nlohmann/json.hpp
