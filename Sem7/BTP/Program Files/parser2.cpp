//#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
//#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/InstrTypes.h"
//#include "llvm/IR/Instructions.h"
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/SourceMgr.h>

using namespace llvm;
using namespace std;
typedef long long ll;
ll intState = 0, intVar = 0;
unordered_map<string,ll> stateMap;
unordered_map<string, string> boolCondMap;
vector<string> highVariables;
bool instructionHandled;

static string getSimpleNodeLabel(const BasicBlock* node) {
    if (!node->getName().empty()) return node->getName().str();
    string Str;
    raw_string_ostream OS(Str);
    node->printAsOperand(OS, false);
    if(stateMap.count(OS.str()) == 0) stateMap[OS.str()] = intState++;
    return "q" + to_string(stateMap[OS.str()]);
}
static string getSimpleOperandName(const Value* node) {
    if (!node->getName().empty()) return node->getName().str();
    string Str;
    raw_string_ostream OS(Str);
    node->printAsOperand(OS, false);
    if(stateMap.count(OS.str()) == 0) stateMap[OS.str()] = intState++;
    return "q" + to_string(stateMap[OS.str()]);
}

string getPredicate(CmpInst::Predicate pred){
    string ans;
    switch(pred){
        case CmpInst::FCMP_FALSE:
            break;
        case CmpInst::FCMP_OEQ:
            break;
        case CmpInst::FCMP_OGT:
            break;
        case CmpInst::FCMP_OGE:
            break;
        case CmpInst::FCMP_OLT:
            break;
        case CmpInst::FCMP_OLE:
            break;
        case CmpInst::FCMP_ONE:
            break;
        case CmpInst::FCMP_ORD:
            break;
        case CmpInst::FCMP_UNO:
            break;
        case CmpInst::FCMP_UEQ:
            break;
        case CmpInst::FCMP_UGT:
            break;
        case CmpInst::FCMP_UGE:
            break;
        case CmpInst::FCMP_ULT:
            break;
        case CmpInst::FCMP_ULE:
            break;
        case CmpInst::FCMP_UNE:
            break;
        case CmpInst::FCMP_TRUE:
            break;
        case CmpInst::BAD_FCMP_PREDICATE:
            break;
        case CmpInst::ICMP_EQ: ans = "==";
            break;
        case CmpInst::ICMP_NE: ans = "!=";
            break;
        case CmpInst::ICMP_UGT:
        case CmpInst::ICMP_SGT: ans = ">";
            break;
        case CmpInst::ICMP_UGE:
        case CmpInst::ICMP_SGE: ans = ">=";
            break;
        case CmpInst::ICMP_ULT:
        case CmpInst::ICMP_SLT: ans = "<";
            break;
        case CmpInst::ICMP_ULE:
        case CmpInst::ICMP_SLE: ans = ">=";
            break;
        case CmpInst::BAD_ICMP_PREDICATE: ans = "??";
            break;
    }
    return ans;
}
string getLHS(Instruction* instIter){
    string resultName;
    if (!instIter->getName().empty()) resultName.assign(instIter->getName().str());
    else{
        string Str;
        raw_string_ostream OS(Str);
        instIter->printAsOperand(OS, false);
        resultName.assign(OS.str());
    }
    if(stateMap.count(resultName) == 0) stateMap[resultName] = intState++;
    return "q" + to_string(stateMap[resultName]);
}

//TODO: only considering Br for now, check for callBr etc. later



int getBranches(BasicBlock* bbIter){
    if(bbIter == nullptr) return 0;
    BasicBlock &basicBlock = *bbIter;
    for (auto instIter = basicBlock.begin(); instIter != basicBlock.end(); instIter++) {
        Instruction &inst = *instIter;
        if(inst.getOpcode() == Instruction::Br){
//            return min(2, (int)inst.getNumOperands());
            return 1;
        }
    }
    return 0;
}

//special cases
void handleBr(Instruction *instIter, ofstream* output){
//    instructionHandled = true;
    if(instIter == nullptr) return;
    if(instIter->getNumOperands() == 1) {
        *output << " " << getSimpleOperandName(instIter->getOperand(0));
        return;
    }
    else{
        *output << " q" << intState << ";\n";
        *output << "q" << intState++ << " 2 ";
        string lhs = getSimpleOperandName(instIter->getOperand(0));
        if(boolCondMap.count(lhs) == 0) return;
        *output  << boolCondMap[lhs] << " | - " << getSimpleOperandName(instIter->getOperand(1)) << "\n";
        int intStateLength = intState > 1 ? (ll)log10(intState-1) + 1 : 1;
        for(int i = 0 ; i < intStateLength; i++) *output << " ";
        *output << "    !" << boolCondMap[lhs]  << " | - " << getSimpleOperandName(instIter->getOperand(2));
    }
}
void handleStore(Instruction *instIter, ofstream* output){
    instructionHandled = true;
    if(instIter) *output<< getSimpleOperandName(instIter->getOperand(1)) << " = " << getSimpleOperandName(instIter->getOperand(0));
}
void handleLoad(Instruction *instIter, ofstream* output){
    instructionHandled = true;
    if(instIter) {
        string operandName = getSimpleOperandName(instIter->getOperand(0));
        *output<< getLHS(instIter) << " = " << operandName;
        highVariables.push_back(operandName);
    }
}
void handleCmp(Instruction* instIter, ofstream* output){
    instructionHandled = true;
    if(instIter == nullptr) return;
    string resultName = getLHS(instIter);
    auto pred = CmpInst::Predicate::BAD_ICMP_PREDICATE;
    if (auto *cmpInst = dyn_cast<CmpInst>(instIter)) {
        pred = cmpInst->getPredicate();
    }
    string rhs = "(" + getSimpleOperandName(instIter->getOperand(0)) + " " + getPredicate(pred) + " " + getSimpleOperandName(instIter->getOperand(1)) + ")";
    boolCondMap[resultName] = rhs;
}
void handleDefault(Instruction* instIter, ofstream* output){
    if(instIter == nullptr) return;
    auto opCodeName = instIter->getOpcodeName();
    *output << opCodeName << "(";
    for(unsigned int i = 0; i < instIter->getNumOperands(); ++i){
        if(instIter->getOpcode() == Instruction::Call && i == 0) continue;
        Value *opnd = instIter->getOperand(i);
        *output << getSimpleOperandName(opnd);
        if(i != instIter->getNumOperands() - 1) *output << ", ";
    }
    *output << ")";
}

//print instruction based on type of opCode
void handleBinaryOps(Instruction* instIter, int opCode, ofstream* output){
    string oper;
    switch(opCode){
        case Instruction::BinaryOpsBegin:
        case Instruction::FAdd: oper = "+";
            break;
        case Instruction::Sub:
        case Instruction::FSub: oper = "-";
            break;
        case Instruction::Mul:
        case Instruction::FMul: oper = "*";
            break;
        case Instruction::UDiv:
        case Instruction::SDiv:
        case Instruction::FDiv: oper = "/";
            break;
        case Instruction::URem:
        case Instruction::SRem:
        case Instruction::FRem: oper = "%";
            break;
        case Instruction::Shl: oper = "<<";
            break;
        case Instruction::LShr:
        case Instruction::AShr: oper = ">>";
            break;
        case Instruction::And: oper = "&";
            break;
        case Instruction::Or: oper = "|";
            break;
        case Instruction::Xor: oper = "^";
            break;
        case Instruction::BinaryOpsEnd: oper = "??";
            break;
    }
    if(oper != "??"){
        instructionHandled = true;
        if(instIter) *output << getLHS(instIter) << " = (" << getSimpleOperandName(instIter->getOperand(0)) << " " << oper << " " << getSimpleOperandName(instIter->getOperand(1)) << ")";

    }
}
void handleUnaryOps(Instruction* instIter, int opCode, ofstream* output){
    switch(opCode){
        case Instruction::UnaryOpsBegin:
            break;
        case Instruction::UnaryOpsEnd:
            break;
    }
}
void handleTermOps(Instruction* instIter, int opCode, ofstream* output){
    switch(opCode){
        case Instruction::TermOpsBegin:
            break;
        case Instruction::Br:
            handleBr(instIter, output);
            break;
        case Instruction::Switch:
            break;
        case Instruction::IndirectBr:
            break;
        case Instruction::Invoke:
            break;
        case Instruction::Resume:
            break;
        case Instruction::Unreachable:
            break;
        case Instruction::CleanupRet:
            break;
        case Instruction::CatchRet:
            break;
        case Instruction::CatchSwitch:
            break;
        case Instruction::Call:
            break;
        case Instruction::TermOpsEnd:
            break;
    }
}
void handleMemoryOps(Instruction* instIter, int opCode, ofstream* output){
    switch(opCode){
        case Instruction::MemoryOpsBegin:
            break;
        case Instruction::Load:
            handleLoad(instIter, output);
            break;
        case Instruction::Store:
            handleStore(instIter, output);
            break;
        case Instruction::GetElementPtr:
            break;
        case Instruction::Fence:
            break;
        case Instruction::AtomicCmpXchg:
            break;
        case Instruction::AtomicRMW:
            break;
        case Instruction::MemoryOpsEnd:
            break;
    }
}
void handleCastOps(Instruction* instIter, int opCode, ofstream* output){
    switch (opCode){
        case Instruction::CastOpsBegin:
            break;
        case Instruction::ZExt:
            break;
        case Instruction::SExt:
            break;
        case Instruction::FPToUI:
            break;
        case Instruction::FPToSI:
            break;
        case Instruction::UIToFP:
            break;
        case Instruction::SIToFP:
            break;
        case Instruction::FPTrunc:
            break;
        case Instruction::FPExt:
            break;
        case Instruction::PtrToInt:
            break;
        case Instruction::IntToPtr:
            break;
        case Instruction::BitCast:
            break;
        case Instruction::AddrSpaceCast:
            break;
        case Instruction::CastOpsEnd:
            break;
    }
}
void handleFuncletPadOps(Instruction* instIter, int opCode, ofstream* output){
    switch(opCode){
        case Instruction::FuncletPadOpsBegin:
            break;
        case Instruction::CatchPad:
            break;
        case Instruction::FuncletPadOpsEnd:
            break;
    }
}
void handleOtherOps(Instruction* instIter, int opCode, ofstream* output){
    switch(opCode){
        case Instruction::ICmp:
        case Instruction::FCmp:
            handleCmp(instIter, output);
            break;
        case Instruction::PHI:
            break;
        case Instruction::Call:
            break;
        case Instruction::Select:
            break;
        case Instruction::UserOp1:
            break;
        case Instruction::UserOp2:
            break;
        case Instruction::VAArg:
            break;
        case Instruction::ExtractElement:
            break;
        case Instruction::InsertElement:
            break;
        case Instruction::ShuffleVector:
            break;
        case Instruction::ExtractValue:
            break;
        case Instruction::InsertValue:
            break;
        case Instruction::LandingPad:
            break;
        case Instruction::OtherOpsEnd:
            break;
        default:
            break;
    }
}

static inline bool isMemoryOp(unsigned OpCode) {
    return OpCode >= Instruction::MemoryOpsBegin && OpCode < Instruction::MemoryOpsEnd;
}
static inline bool isTermOp(unsigned OpCode) {
    return OpCode >= Instruction::TermOpsBegin && OpCode < Instruction::TermOpsEnd;
}
static inline bool isOtherOp(unsigned OpCode) {
    return OpCode >= Instruction::OtherOpsBegin && OpCode < Instruction::OtherOpsEnd;
}

bool getComma(BasicBlock::const_iterator instr, BasicBlock* bbIter){
    if(bbIter == NULL) return false;
    int opCode = instr->getOpcode();
    bool comma = (opCode != Instruction::Br);
    comma &= !(opCode == Instruction::ICmp || opCode == Instruction::FCmp);
    comma &= (instr++ != bbIter->end() && !(instr->getOpcode() == Instruction::Br || instr->getOpcode() == Instruction::ICmp || instr->getOpcode() == Instruction::FCmp));
    return comma;
}

bool handleInstruction(Instruction* instIter, ofstream* output){
    instructionHandled = false;
    if(instIter == nullptr || output == nullptr) return false;
    auto opCode = instIter->getOpcode();
    if(Instruction::isBinaryOp(opCode)) handleBinaryOps(instIter, opCode, output);
    else if(Instruction::isUnaryOp(opCode)) handleUnaryOps(instIter, opCode, output);
    else if(Instruction::isCast(opCode)) handleCastOps(instIter, opCode, output);
    else if(Instruction::isFuncletPad(opCode)) handleFuncletPadOps(instIter, opCode, output);
    else if(isTermOp(opCode)) handleTermOps(instIter, opCode, output);
    else if(isMemoryOp(opCode)) handleMemoryOps(instIter, opCode, output);
    else if(isOtherOp(opCode)) handleOtherOps(instIter, opCode, output);
//    if(!instructionHandled) handleDefault(instIter, output);
    if(!instructionHandled) cout<<instIter->getOpcode()<<" not handled\n";
    return instructionHandled;
}
void handleBasicBlock(BasicBlock* bbIter, ofstream* output){
    if(bbIter == nullptr || output == nullptr) return;
    BasicBlock &basicBlock = *bbIter;
    int branches = getBranches(bbIter);
    *output << getSimpleNodeLabel(&basicBlock) << " " << branches ;
    if(branches > 0) *output << " - | ";
    bool comma = false;
    for (auto instIter = basicBlock.begin(); instIter != basicBlock.end(); instIter++) {
        Instruction &inst = *instIter;
        if(comma) *output << ", ";
        comma = handleInstruction(&inst, output);
        comma &= getComma(instIter, bbIter);
        /*          TODO: Fix this comma issue
           Instruction &inst = *instIter;
           bool cur = handleInstruction(&inst, output);
           if(cur && comma) *output << ", ";
           if(cur) comma &= getComma(instIter, bbIter);
       }
       */
    }
    *output << " ;\n";
}
void generateFSMD(unique_ptr<Module>& module){
    ofstream output;
    output.open("output.txt");
    output << "\"output.txt\"\n";
    if(!module) {
        output.close();
        return;
    }
    for (auto funcIter = module->getFunctionList().begin(); funcIter != module->getFunctionList().end(); funcIter++) {
        Function &function = *funcIter;
        cout << " Function: " << function.getName().str() << endl;

        for (auto bbIter = function.getBasicBlockList().begin(); bbIter != function.getBasicBlockList().end(); bbIter++) {
            BasicBlock &basicBlock = *bbIter;
            handleBasicBlock(&basicBlock, &output);
        }
    }
    output.close();
    output.open("variables.txt");
    for(auto x : highVariables) output << x << endl;
    output.close();
}

void printPattern(unique_ptr<Module>& module){
    if (!module) return;
    for (auto funcIter = module->getFunctionList().begin(); funcIter != module->getFunctionList().end(); funcIter++) {
        Function &function = *funcIter;
        cout << " Function: " << function.getName().str() << endl;

        for (auto bbIter = function.getBasicBlockList().begin(); bbIter != function.getBasicBlockList().end(); bbIter++) {
            BasicBlock &basicBlock = *bbIter;
            cout << "  Label: " << getSimpleNodeLabel(&basicBlock) << endl;

            for (auto instIter = basicBlock.begin(); instIter != basicBlock.end(); instIter++) {
                Instruction &inst = *instIter;
                auto opCodeName = inst.getOpcodeName();
                if(inst.getOpcode() == Instruction::Alloca) continue;
                cout << "     " << opCodeName;
                if(inst.getOpcode() == Instruction::ICmp || inst.getOpcode() == Instruction::FCmp){
                    if (CmpInst *cmpInst = dyn_cast<CmpInst>(instIter)) {
                        auto predicateType = cmpInst->getPredicateName(cmpInst->getPredicate());
                        cout << " " << predicateType.str();
                    }
                }
                for(unsigned int i = 0; i < inst.getNumOperands(); ++i){
                    if(inst.getOpcode() == Instruction::Call && i == 0) continue;
                    Value *opnd = inst.getOperand(i);
                    cout << " " << getSimpleOperandName(opnd) << ",";
                }
                cout << endl;
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        errs() << "Usage: " << argv[0] << " <IR file>\n";
        return 1;
    }

    // Parse the input LLVM IR file into a module.
    SMDiagnostic err;
    LLVMContext context;
    unique_ptr<Module> module(parseIRFile(argv[1], err, context));
    printPattern(module);
    generateFSMD(module);
    return 0;
}

//clang++ parser.cpp -o reader `llvm-config --cxxflags --libs --ldflags --system-libs`