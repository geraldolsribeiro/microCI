test:
	../src/microCI -i y.yml      || echo "OK"
	../src/microCI --input=x.yml || echo "OK"
	../src/microCI -h > help.txt
	../src/microCI > default.sh
	../src/microCI -i ../new/git_deploy.yml > git_deploy.sh
	../src/microCI -i ../new/mkdocs_material.yml > mkdocs_material.sh
	../src/microCI -i ../new/mkdocs_material.yml -O "local-step" > mkdocs_material_only.sh
	../src/microCI -c tipo_invalido || echo "OK"
	../src/microCI -c npm -i test_npm.yml
	../src/microCI -c git_deploy -i test_git_deploy.yml
	../src/microCI -c mkdocs_material -i test_mkdocs_material.yml
