# source/demos/Makefile
#
# Make all demos or an individual demo
#
# Example: make
# Example: make VAR_Demo
#
# The compiled executable will be copied to source/demos

IC = ../../bin/icompile

DEMOS = ASM_Shader_Demo Cg_Shader_Demo Collision_Demo GLG3D_Demo GLProgram_Demo GLSL_Demo MD2Model_Demo Network_Demo ParallaxBump_Demo VAR_Demo

all: $(DEMOS)

.PHONY: $(DEMOS)
$(DEMOS): 
	@echo "Building $@"; \
	cd $@; \
        chmod u+x $(IC); \
	$(IC); \
	cp -f '_build/distrib/$@d' ../; \
	cd ../;

