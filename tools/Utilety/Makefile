CXX=x86_64-w64-mingw32-g++

EXE_NAME	:= Utilety.exe

export ROOT	?= $(CURDIR)

export PROJ	?= $(notdir $(CURDIR))
TITLE		:= $(PROJ)
BUILD		:= build
SRCDIRS		:= source
DATADIRS	:= data
INCDIRS		:= include
EXE			:= $(CURDIR)/$(EXE_NAME)

CXXFLAGS	:= -O2 $(INCLUDE) -Wall -Wextra -pedantic-errors -std=c++2a


ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=									\
	$(foreach dir, $(SRCDIRS) , $(CURDIR)/$(dir))	\
	$(foreach dir, $(DATADIRS), $(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir, $(SRCDIRS) , $(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir, $(SRCDIRS) , $(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir, $(SRCDIRS) , $(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir, $(DATADIRS), $(notdir $(wildcard $(dir)/*.*)))

export LD	:= $(CXX)

# --- Define object file list ---
export OFILES	:=	$(addsuffix .o, $(BINFILES))					\
					$(CFILES:.c=.o) $(CPPFILES:.cpp=.o)				\
					$(SFILES:.s=.o)

# --- Create include and library search paths ---
export INCLUDE	:=	$(foreach dir,$(INCDIRS),-I$(CURDIR)/$(dir))	\
					$(foreach dir,$(LIBDIRS),-I$(dir)/include)		\
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	-L$(CURDIR) $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(EXE): $(BUILD)
	$(CXX) -o $(EXE) $(OFILES)

all	: $(BUILD)

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).out

else		# If we're here, we should be in the BUILD dir

# --- Main targets ----

$(TARGET)	:	$(CXX) $(CXXFLAGS) -o $(TARGET).exe $(OFILES)

$(OUTPUT)	:	$(OFILES)
	$(CXX) -o $(EXE) $(OFILES)


endif		# End BUILD switch
