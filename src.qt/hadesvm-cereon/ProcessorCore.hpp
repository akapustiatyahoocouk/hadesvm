//
//  hadesvm-cereon/ProcessorCore.hpp
//
//  Cereon processor core
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  Optional features
        enum class Feature
        {
            Base = 0x01,
            FloatingPoint = 0x02,
            Debug = 0x04,
            UnalignedOperand = 0x08,
            ProtectedMemory = 0x10,
            VirtualMemory = 0x20,
            PerformanceMonitoring = 0x40
        };

        class HADESVM_CEREON_PUBLIC Features final
        {
            //////////
            //  Construction/destruction/assignment
        private:
            Features(int mask) : _mask(mask) { _normalize(); }

        public:
            constexpr Features() : _mask(0) {}
            constexpr Features(Feature feature) : _mask(static_cast<int>(feature)) {}

            //////////
            //  Operators
        public:
            Features &          operator += (Features & op2);
            Features &          operator -= (Features & op2);

            Features            operator + (const Features & op2) const;
            Features            operator - (const Features & op2) const;

            //////////
            //  Operations
        public:
            bool                has(Feature feature) const { return (_mask & static_cast<int>(feature)) != 0; }

            //////////
            //  Implementation
        private:
            int                 _mask;  //  of features

            //  Helpers
            void                _normalize();
        };

        inline Features operator + (Feature op1, Feature op2)
        {
            return Features(op1) + Features(op2);
        }

        inline Features operator + (Feature op1, const Features & op2)
        {
            return Features(op1) + op2;
        }

        //////////
        //  Thrown by instruction handling routines when a PROGRAM interrupt (exception) occurs
        class HADESVM_CEREON_PUBLIC ProgramInterrupt final
        {
            //////////
            //  Constants
        public:
            static const uint64_t   ZDIV        = UINT64_C(0x0000000000000001);
            static const uint64_t   IADDRESS    = UINT64_C(0x0000000000000002);
            static const uint64_t   DADDRESS    = UINT64_C(0x0000000000000003);
            static const uint64_t   IACCESS     = UINT64_C(0x0000000000000004);
            static const uint64_t   DACCESS     = UINT64_C(0x0000000000000005);
            static const uint64_t   IALIGN      = UINT64_C(0x0000000000000006);
            static const uint64_t   DALIGN      = UINT64_C(0x0000000000000007);
            static const uint64_t   OPCODE      = UINT64_C(0x0000000000000008);
            static const uint64_t   OPERAND     = UINT64_C(0x0000000000000009);
            static const uint64_t   PRIVILEGED  = UINT64_C(0x000000000000000A);
            static const uint64_t   PAGETABLE   = UINT64_C(0x000000000000000B);
            static const uint64_t   IPAGEFAULT  = UINT64_C(0x000000000000000C);
            static const uint64_t   DPAGEFAULT  = UINT64_C(0x000000000000000D);
            static const uint64_t   TRAP        = UINT64_C(0x000000000000000E);
            static const uint64_t   MASKED      = UINT64_C(0x000000000000000F);
            static const uint64_t   IOVERFLOW   = UINT64_C(0x0000000000000010);
            static const uint64_t   FOPERAND    = UINT64_C(0x0000000000000011);
            static const uint64_t   FZDIV       = UINT64_C(0x0000000000000012);
            static const uint64_t   FOVERFLOW   = UINT64_C(0x0000000000000013);
            static const uint64_t   FUNDERFLOW  = UINT64_C(0x0000000000000014);
            static const uint64_t   FINEXACT    = UINT64_C(0x0000000000000015);
            static const uint64_t   DEBUG0      = UINT64_C(0x0000000000000016);
            static const uint64_t   BREAK0      = UINT64_C(0x0000000000000017);
            static const uint64_t   UNKNOWN     = UINT64_C(0xFFFFFFFFFFFFFFFF);

            //////////
            //  Construction/destruction/assignment
        public:
            explicit ProgramInterrupt(uint64_t isc) : interruptStatusCode(isc) {}

            //////////
            //  Properties
        public:
            const uint64_t      interruptStatusCode;
        };

        //////////
        //  Thrown by instruction handling routines when a HARDWARE interrupt (exception) occurs
        class HADESVM_CEREON_PUBLIC HardwareInterrupt final
        {
            //////////
            //  Constants
        public:
            static const uint64_t   PROCESSOR   = UINT64_C(0x0000000000000001);
            static const uint64_t   MEMORY      = UINT64_C(0x0000000000000002);
            static const uint64_t   BUS         = UINT64_C(0x0000000000000003);
            static const uint64_t   IO          = UINT64_C(0x0000000000000004);
            static const uint64_t   TIMER       = UINT64_C(0x0000000000000005);
            static const uint64_t   UNKNOWN     = UINT64_C(0xFFFFFFFFFFFFFFFF);

            //////////
            //  Construction/destruction/assignment
        public:
            explicit HardwareInterrupt(uint64_t isc) : interruptStatusCode(isc) {}

            //////////
            //  Properties
        public:
            const uint64_t      interruptStatusCode;
        };

        //////////
        //  Thrown by instruction handling routines when CPU core must be HALTed
        class HADESVM_CEREON_PUBLIC ForceHalt final
        {
        };

        //////////
        //  An abstract interface to a MMU (for address translation & access control)
        class HADESVM_CEREON_PUBLIC Mmu
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Mmu)

            //////////
            //  Construction/destruction
        protected:
            Mmu() = default;
            virtual ~Mmu() = default;

            //////////
            //  Operations
        public:
            //  Translates logical address (of an instruction fetch, data
            //  load or data store) to physical address.
            //  Throws MemoryAccessError if the translation fails.
            virtual uint64_t    translateFetchAddress(uint64_t logicalAddress) throws(MemoryAccessError) = 0;
            virtual uint64_t    translateLoadAddress(uint64_t logicalAddress) throws(MemoryAccessError)= 0;
            virtual uint64_t    translateStoreAddress(uint64_t logicalAddress) throws(MemoryAccessError)= 0;
        };

        //////////
        //  A "dummy" MMU (no address translation)
        class HADESVM_CEREON_PUBLIC DummyMmu : public Mmu
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(DummyMmu)

            //////////
            //  Construction/destruction
        public:
            DummyMmu() = default;

            //////////
            //  Mmu
        public:
            virtual uint64_t    translateFetchAddress(uint64_t logicalAddress) throws(MemoryAccessError) override { return logicalAddress; }
            virtual uint64_t    translateLoadAddress(uint64_t logicalAddress) throws(MemoryAccessError) override { return logicalAddress; }
            virtual uint64_t    translateStoreAddress(uint64_t logicalAddress) throws(MemoryAccessError) override { return logicalAddress; }
        };

        //////////
        //  A single processor core
        class HADESVM_CEREON_PUBLIC ProcessorCore
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ProcessorCore)

            friend class Processor;

            //////////
            //  Construction/destruction
        public:
            ProcessorCore(Processor * processor, uint8_t id, const Features & features,
                          hadesvm::util::ByteOrder initialByteOrder, bool canChangeByteOrder,
                          Mmu * mmu);
            virtual ~ProcessorCore() noexcept;

            //////////
            //  Operations
        public:
            //  Resets this processor core to post-bootstrapping state
            void                    reset();

            //  Called on every clock tick
            void                    onClockTick();

            //////////
            //  Implementation
        private:
            Processor *const        _processor; //  ...to which this core belongs
            const uint8_t           _id;        //  ...of this core
            const Features          _features;  //  ...of this core
            const hadesvm::util::ByteOrder  _initialByteOrder;
            const bool              _canChangeByteOrder;
            const bool              _isPrimaryCore;

            //  Links to other processor components
            Mmu *                   _mmu;

            //  Registers & state
            class _StateRegister final
            {
                //////////
                //  Constants
            public:
                static const uint64_t   HardwareInterruptsEnabledMask = UINT64_C(0x0000000080000000);
                static const uint64_t   ExternalInterruptsEnabledMask = UINT64_C(0x0000000040000000);
                static const uint64_t   ProgramInterruptsEnabledMask = UINT64_C(0x0000000020000000);
                static const uint64_t   SvcInterruptsEnabledMask = UINT64_C(0x0000000010000000);
                static const uint64_t   IoInterruptsEnabledMask = UINT64_C(0x0000000008000000);
                static const uint64_t   TimerInterruptsEnabledMask = UINT64_C(0x0000000004000000);

                static const uint64_t   KernelModeMask = UINT64_C(0x0000000000000001);
                static const uint64_t   VirtualModeMask = UINT64_C(0x0000000000000002);
                static const uint64_t   TrapMask = UINT64_C(0x0000000000000004);
                static const uint64_t   PendingTrapMask = UINT64_C(0x0000000000000008);
                static const uint64_t   BigEndianModeMask = UINT64_C(0x0000000000000010);
                static const uint64_t   WorkingModeMask = UINT64_C(0x0000000000000020);
                static const uint64_t   DebugEventsMask = UINT64_C(0x0000000000000040);
                static const uint64_t   RealOperandExceptionMask = UINT64_C(0x0000000000000080);
                static const uint64_t   RealDivisionByZeroExceptionMask = UINT64_C(0x0000000000000100);
                static const uint64_t   RealOverflowExceptionMask = UINT64_C(0x0000000000000200);
                static const uint64_t   RealUnderflowExceptionMask = UINT64_C(0x0000000000000400);
                static const uint64_t   RealInexactExceptionMask = UINT64_C(0x0000000000000800);
                static const uint64_t   IntegerDivisionByZeroExceptionMask = UINT64_C(0x0000000000001000);
                static const uint64_t   IntegerOverflowExceptionMask = UINT64_C(0x0000000000002000);

                //////////
                //  Construction/destruction/assignment
            public:
                _StateRegister() : _value(0) { _replicateBitFields(); }
                explicit _StateRegister(uint64_t value) : _value(value) { _replicateBitFields(); }

                //////////
                //  Operators
            public:
                _StateRegister &    operator = (uint64_t value) { _value = value; _replicateBitFields(); return *this; }
                                    operator uint64_t () const { return _value; }

                //////////
                //  Operations
            public:
                uint64_t &          value() { return _value; }
                bool                isInKernelMode() const { return (_value & KernelModeMask) != 0; }
                bool                isInUserMode() const { return (_value & KernelModeMask) == 0; }
                void                setKernelMode() { _value |= KernelModeMask; _replicateBitFields(); }
                bool                isInRealMode() const { return !_virtualMode; }
                bool                isInVirtualMode() const { return _virtualMode; }
                bool                isInTrapMode() const { return _trapMode; }
                void                setTrapMode() { _value |= TrapMask; _replicateBitFields(); }
                bool                isInPendingTrapMode() const { return _pendingTrapMode; }
                void                clearPendingTrapMode() { _value &= ~PendingTrapMask; _replicateBitFields(); }
                hadesvm::util::ByteOrder    getByteOrder() const { return _byteOrder; }
                void                setByteOrder(hadesvm::util::ByteOrder byteOrder)
                {
                    _value = (byteOrder == hadesvm::util::ByteOrder::BigEndian) ?
                                (_value | BigEndianModeMask) :
                                (_value & ~BigEndianModeMask);
                    _replicateBitFields();
                }
                bool                isInWorkingMode() const { return _workingMode; }
                bool                isInIdleMode() const { return !_workingMode; }
                void                setWorkingMode() { _value |= WorkingModeMask; _replicateBitFields(); }
                void                setIdleMode() { _value &= ~WorkingModeMask; _replicateBitFields(); }

                bool                isDebugEventsAllowed() const { return (_value & DebugEventsMask) != 0; }
                bool                isRealOperandExceptionAllowed() const { return (_value & RealOperandExceptionMask) != 0; }
                bool                isRealDivisionByZeroExceptionAllowed() const { return (_value & RealDivisionByZeroExceptionMask) != 0; }
                bool                isRealOverflowExceptionAllowed() const { return (_value & RealOverflowExceptionMask) != 0; }
                bool                isRealUnderflowExceptionAllowed() const { return (_value & RealUnderflowExceptionMask) != 0; }
                bool                isRealInexactExceptionAllowed() const { return (_value & RealInexactExceptionMask) != 0; }
                bool                isIntegerDivisionByZeroExceptionAllowed() const { return (_value & IntegerDivisionByZeroExceptionMask) != 0; }
                bool                isIntegerOverflowExceptionAllowed() const { return (_value & IntegerOverflowExceptionMask) != 0; }

                bool                isHardwareInterruptsEnabled() const { return (_value & HardwareInterruptsEnabledMask) != 0; }
                void                enableHardwareInterrupts() { _value |= HardwareInterruptsEnabledMask; }
                bool                isExternalInterruptsEnabled() const { return (_value & ExternalInterruptsEnabledMask) != 0; }
                void                enableExternalInterrupts() { _value |= ExternalInterruptsEnabledMask; }
                bool                isProgramInterruptsEnabled() const { return (_value & ProgramInterruptsEnabledMask) != 0; }
                void                enableProgramInterrupts() { _value |= ProgramInterruptsEnabledMask; }
                bool                isSvcInterruptsEnabled() const { return (_value & SvcInterruptsEnabledMask) != 0; }
                void                enableSvcInterrupts() { _value |= SvcInterruptsEnabledMask; }
                bool                isIoInterruptsEnabled() const { return (_value & IoInterruptsEnabledMask) != 0; }
                void                enableIoInterrupts() { _value |= IoInterruptsEnabledMask; }
                bool                isTimerInterruptsEnabled() const { return (_value & TimerInterruptsEnabledMask) != 0; }
                void                enableTimerInterrupts() { _value |= TimerInterruptsEnabledMask; }

                uint32_t            getContextId() const { return _contextId; }

                //////////
                //  Implementation
            private:
                uint64_t            _value; //  the underlying control register

                //  Most frequently used $state fields - replicated for faster access
                bool                _virtualMode = false;
                bool                _trapMode = false;
                bool                _pendingTrapMode = false;
                bool                _workingMode = false;
                hadesvm::util::ByteOrder    _byteOrder = hadesvm::util::ByteOrder::Unknown; //  replicates B bit of $state for faster access
                uint32_t            _contextId = 0; //  replicates CID bits of $state for faster access

                //  Helpers
                void                _replicateBitFields()
                {
                    _virtualMode = (_value & VirtualModeMask) != 0;
                    _trapMode = (_value & TrapMask) != 0;
                    _pendingTrapMode = (_value & PendingTrapMask) != 0;
                    _workingMode = (_value & WorkingModeMask) != 0;
                    _byteOrder = ((_value & BigEndianModeMask) != 0) ? hadesvm::util::ByteOrder::BigEndian : hadesvm::util::ByteOrder::LittleEndian;
                    _contextId = static_cast<uint32_t>(_value >> 32);;
                    //  TODO the rest of them
                }
            };

            uint64_t            _r[32];

            _StateRegister      _state;
            uint64_t            _pth;
            uint64_t            _itc;
            uint64_t            _cc;
            uint64_t            _isaveipTm;
            uint64_t            _isavestateTm;
            uint64_t            _ihstateTm;
            uint64_t            _ihaTm;
            uint64_t            _isaveipIo;
            uint64_t            _isavestateIo;
            uint64_t            _ihstateIo;
            uint64_t            _ihaIo;
            uint64_t            _iscIo;
            uint64_t            _isaveipSvc;
            uint64_t            _isavestateSvc;
            uint64_t            _ihstateSvc;
            uint64_t            _ihaSvc;
            uint64_t            _isaveipPrg;
            uint64_t            _isavestatePrg;
            uint64_t            _ihstatePrg;
            uint64_t            _ihaPrg;
            uint64_t            _iscPrg;
            uint64_t            _isaveipExt;
            uint64_t            _isavestateExt;
            uint64_t            _ihstateExt;
            uint64_t            _ihaExt;
            uint64_t            _iscExt;
            uint64_t            _isaveipHw;
            uint64_t            _isavestateHw;
            uint64_t            _ihstateHw;
            uint64_t            _ihaHw;
            uint64_t            _iscHw;
            uint64_t *const     _cPtr[32];

            uint64_t            _d[32];
            uint64_t            _m[32];

            class HADESVM_CEREON_PUBLIC _FlagsRegister final
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_FlagsRegister)

                //////////
                //  Constant
            public:
                static const uint64_t   RealOperandFlag = 0x0001;
                static const uint64_t   RealDivisionByZeroFlag = 0x0002;
                static const uint64_t   RealOverflowFlag = 0x0004;
                static const uint64_t   RealUnderflowFlag = 0x0008;
                static const uint64_t   RealInexactFlag = 0x0010;
                static const uint64_t   IntegerDivisionByZeroFlag = 0x0020;
                static const uint64_t   IntegerOverflowFlag = 0x0040;

                //////////
                //  Construction/destruction
            public:
                explicit _FlagsRegister() : _value(0) {}

                //////////
                //  Operators
            public:
                _FlagsRegister &operator = (uint64_t value) { _value = value; return *this; }
                                operator uint64_t() const { return _value; }

                //////////
                //  Operations
            public:
                void            setRealOperandFlag() { _value |= RealOperandFlag; }
                void            setRealDivisionByZeroFlag() { _value |= RealDivisionByZeroFlag; }
                void            setRealOverflowFlag() { _value |= RealOverflowFlag; }
                void            setRealUnderflowFlag() { _value |= RealUnderflowFlag; }
                void            setRealInexactFlag() { _value |= RealInexactFlag; }
                void            setIntegerDivisionByZeroFlag() { _value |= IntegerDivisionByZeroFlag; }
                void            setIntegerOverflowFlag() { _value |= IntegerOverflowFlag; }

                //////////
                //  Implementation
            private:
                uint64_t        _value;
            };
            _FlagsRegister      _flags;

            //  Special names for special registers
            static const unsigned   _RaRegister = 30;
            static const unsigned   _IpRegister = 31;

            //  The number of clock cycles to stall for (emulating multi-cycle instructions)
            unsigned            _cyclesToStall;

            //  "Data type" definitions provide operand/result conversions
            //  for various flavours of arithmetic/logical/shift instructions
            struct _Byte
            {   //  Conversions required by "[un]signed byte" data type
                static uint64_t normalizeSigned(uint64_t operand)
                {
                    return Alu::signExtendInt8(operand);
                }

                static uint64_t normalizeSigned(uint64_t operand, bool & overflow)
                {
                    uint64_t result = Alu::signExtendInt8(operand);
                    if (result != operand)
                    {
                        overflow = true;
                    }
                    return result;
                }

                static uint64_t normalizeUnsigned(uint64_t operand)
                {
                    return Alu::zeroExtendInt8(operand);
                }

                static uint64_t normalizeUnsigned(uint64_t operand, bool & overflow)
                {
                    uint64_t result = Alu::zeroExtendInt8(operand);
                    if (result != operand)
                    {
                        overflow = true;
                    }
                    return result;
                }

                static uint64_t reverseBits(uint64_t operand)
                {
                    return Alu::reverseBits8(operand);
                }
            };

            struct _HalfWord
            {   //  Conversions required by "[un]signed half-word" data type
                static uint64_t normalizeSigned(uint64_t operand)
                {
                    return Alu::signExtendInt16(operand);
                }

                static uint64_t normalizeSigned(uint64_t operand, bool & overflow)
                {
                    uint64_t result = Alu::signExtendInt16(operand);
                    if (result != operand)
                    {
                        overflow = true;
                    }
                    return result;
                }

                static uint64_t normalizeUnsigned(uint64_t operand)
                {
                    return Alu::zeroExtendInt16(operand);
                }

                static uint64_t normalizeUnsigned(uint64_t operand, bool & overflow)
                {
                    uint64_t result = Alu::zeroExtendInt16(operand);
                    if (result != operand)
                    {
                        overflow = true;
                    }
                    return result;
                }

                static uint64_t reverseBits(uint64_t operand)
                {
                    return Alu::reverseBits16(operand);
                }
            };

            struct _Word
            {   //  Conversions required by "[un]signed word" data type
                static uint64_t normalizeSigned(uint64_t operand)
                {
                    return Alu::signExtendInt32(operand);
                }

                static uint64_t normalizeSigned(uint64_t operand, bool & overflow)
                {
                    uint64_t result = Alu::signExtendInt32(operand);
                    if (result != operand)
                    {
                        overflow = true;
                    }
                    return result;
                }

                static uint64_t normalizeUnsigned(uint64_t operand)
                {
                    return Alu::zeroExtendInt32(operand);
                }

                static uint64_t normalizeUnsigned(uint64_t operand, bool & overflow)
                {
                    uint64_t result = Alu::zeroExtendInt32(operand);
                    if (result != operand)
                    {
                        overflow = true;
                    }
                    return result;
                }

                static uint64_t reverseBits(uint64_t operand)
                {
                    return Alu::reverseBits32(operand);
                }
            };

            struct _LongWord
            {   //  Conversions required by "long word" data type
                static uint64_t normalizeSigned(uint64_t operand) { return operand; }
                static uint64_t normalizeSigned(uint64_t operand, bool & /*overflow*/) { return operand; }
                static uint64_t normalizeUnsigned(uint64_t operand) { return operand; }
                static uint64_t normalizeUnsigned(uint64_t operand, bool & /*overflow*/) { return operand; }
                static uint64_t reverseBits(uint64_t operand) { return Alu::reverseBits64(operand); }
            };

            //  Helpers (memory access)
            //  Can throw ProgramInterrupt, HardwareInterrupt, ForceHalt
            uint32_t            _fetchInstruction(uint64_t address);
            uint64_t            _fetchLongWord(uint64_t address);
            uint8_t             _loadByte(uint64_t address);
            uint16_t            _loadHalfWord(uint64_t address);
            uint32_t            _loadWord(uint64_t address);
            uint64_t            _loadLongWord(uint64_t address);
            void                _storeByte(uint64_t address, uint8_t value);
            void                _storeHalfWord(uint64_t address, uint16_t value);
            void                _storeWord(uint64_t address, uint32_t value);
            void                _storeLongWord(uint64_t address, uint64_t value);

            //  Helpers (interrupt handling)
            [[noreturn]] void   _translateAndThrow(MemoryAccessError memoryAccessError);
            void                _handleTimerInterrupt();
            void                _handleIoInterrupt(uint64_t interruptStatusCode);
            void                _handleSvcInterrupt();
            void                _handleProgramInterrupt(uint64_t interruptStatusCode);
            void                _handleExternalInterrupt(uint64_t interruptStatusCode);
            void                _handleHardwareInterrupt(uint64_t interruptStatusCode);

            void                _raiseProgramInterrupt(uint64_t interruptStatusCode);   //  throws ForceHalt if masked
            void                _raiseHardwareInterrupt(uint64_t interruptStatusCode);  //  throws ForceHalt if masked

            //  Helpers (instruction execution). Each handler returns the
            //  number of clock cycles taken by instruction execution.
            //  Can throw ProgramInterrupt, HardwareInterrupt, ForceHalt
            typedef unsigned (ProcessorCore::*_InstructionHandler)(uint32_t instruction);

            unsigned                _fetchAndExecuteInstruction();
            unsigned                _handleInvalidInstruction(uint32_t instruction);
            unsigned                _handleCop1(uint32_t instruction);
            unsigned                _handleShift1(uint32_t instruction);
            unsigned                _handleShift2(uint32_t instruction);
            unsigned                _handleBase1(uint32_t instruction);
            unsigned                _handleBase2(uint32_t instruction);
            unsigned                _handleBase3(uint32_t instruction);
            unsigned                _handleBase4(uint32_t instruction);
            unsigned                _handleBase5(uint32_t instruction);
            unsigned                _handleFp1(uint32_t instruction);

            //  BASE (data movement)
            unsigned                _handleMovL(uint32_t instruction);
            unsigned                _handleMovCR(uint32_t instruction);
            unsigned                _handleMovRC(uint32_t instruction);
            unsigned                _handleLiL(uint32_t instruction);
            unsigned                _handleSwapH(uint32_t instruction);
            unsigned                _handleSwapUH(uint32_t instruction);
            unsigned                _handleSwapW(uint32_t instruction);
            unsigned                _handleSwapUW(uint32_t instruction);
            unsigned                _handleSwapL(uint32_t instruction);

            //  BASE (arithmetic)
            void                    _handleIntegerOverflow(bool overflow)
            {
                if (overflow)
                {
                    _flags.setIntegerOverflowFlag();
                    //  Exceptions ?
                    if (_state.isIntegerOverflowExceptionAllowed())
                    {   //  PROGRAM interrupt if possible, HALT if masked
                        _raiseProgramInterrupt(ProgramInterrupt::IOVERFLOW);
                    }
                }
            }

            void                    _handleIntegerDivisionByZero(bool divisionByZero)
            {
                if (divisionByZero)
                {
                    _flags.setIntegerDivisionByZeroFlag();
                    //  Exceptions ?
                    if (_state.isIntegerDivisionByZeroExceptionAllowed())
                    {   //  PROGRAM interrupt if possible, HALT if masked
                        _raiseProgramInterrupt(ProgramInterrupt::ZDIV);
                    }
                }
            }

            template <class DataType>
            void                    _executeAddSigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false;
                result =
                    DataType::normalizeSigned(
                        Alu::addSigned(DataType::normalizeSigned(op1), DataType::normalizeSigned(op2), overflow),
                        overflow);
                _handleIntegerOverflow(overflow);
            }

            template <class DataType>
            void                    _executeAddUnsigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false;
                result =
                    DataType::normalizeUnsigned(
                        Alu::addUnsigned(DataType::normalizeUnsigned(op1), DataType::normalizeUnsigned(op2), overflow),
                        overflow);
                _handleIntegerOverflow(overflow);
            }

            template <class DataType>
            void                    _executeSubtractSigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false;
                result =
                    DataType::normalizeSigned(
                        Alu::subtractSigned(DataType::normalizeSigned(op1), DataType::normalizeSigned(op2), overflow),
                        overflow);
                _handleIntegerOverflow(overflow);
            }

            template <class DataType>
            void                    _executeSubtractUnsigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false;
                result =
                    DataType::normalizeUnsigned(
                        Alu::subtractUnsigned(DataType::normalizeUnsigned(op1), DataType::normalizeUnsigned(op2), overflow),
                        overflow);
                _handleIntegerOverflow(overflow);
            }

            template <class DataType>
            void                    _executeMultiplySigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false;
                result =
                    DataType::normalizeSigned(
                        Alu::multiplySigned(DataType::normalizeSigned(op1), DataType::normalizeSigned(op2), overflow),
                        overflow);
                _handleIntegerOverflow(overflow);
            }

            template <class DataType>
            void                    _executeMultiplyUnsigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false;
                result =
                    DataType::normalizeUnsigned(
                        Alu::multiplyUnsigned(DataType::normalizeUnsigned(op1), DataType::normalizeUnsigned(op2), overflow),
                        overflow);
                _handleIntegerOverflow(overflow);
            }

            template <class DataType>
            void                    _executeDivideSigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false, divisionByZero = false;
                result =
                    DataType::normalizeSigned(
                        Alu::divideSigned(DataType::normalizeSigned(op1), DataType::normalizeSigned(op2), overflow, divisionByZero),
                        overflow);
                _handleIntegerDivisionByZero(divisionByZero);
                _handleIntegerOverflow(overflow);
            }

            template <class DataType>
            void                    _executeDivideUnsigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false, divisionByZero = false;
                result =
                    DataType::normalizeUnsigned(
                        Alu::divideUnsigned(DataType::normalizeUnsigned(op1), DataType::normalizeUnsigned(op2), divisionByZero),
                        overflow);
                _handleIntegerDivisionByZero(divisionByZero);
                _handleIntegerOverflow(overflow);
            }

            template <class DataType>
            void                    _executeModuloSigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false, divisionByZero = false;
                result =
                    DataType::normalizeSigned(
                        Alu::moduloSigned(DataType::normalizeSigned(op1), DataType::normalizeSigned(op2), divisionByZero),
                        overflow);
                Q_ASSERT(!overflow);
                _handleIntegerDivisionByZero(divisionByZero);
            }

            template <class DataType>
            void                    _executeModuloUnsigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                bool overflow = false, divisionByZero = false;
                result =
                    DataType::normalizeUnsigned(
                        Alu::moduloUnsigned(DataType::normalizeUnsigned(op1), DataType::normalizeUnsigned(op2), divisionByZero),
                        overflow);
                Q_ASSERT(!overflow);
                _handleIntegerDivisionByZero(divisionByZero);
            }

            template <class DataType>
            void                    _executeAbsSigned(uint64_t & result, uint64_t op)
            {
                bool overflow = false;
                result =
                    DataType::normalizeSigned(
                        Alu::absSigned(DataType::normalizeSigned(op), overflow),
                        overflow);
                _handleIntegerOverflow(overflow);
            }

            template <class DataType>
            void                    _executeCpl2Unsigned(uint64_t & result, uint64_t op)
            {
                bool overflow = false;
                result =
                    DataType::normalizeUnsigned(
                        Alu::subtractUnsigned(0, DataType::normalizeUnsigned(op), overflow),
                        overflow);
            }

            unsigned                _handleAddB(uint32_t instruction);
            unsigned                _handleAddUB(uint32_t instruction);
            unsigned                _handleAddH(uint32_t instruction);
            unsigned                _handleAddUH(uint32_t instruction);
            unsigned                _handleAddW(uint32_t instruction);
            unsigned                _handleAddUW(uint32_t instruction);
            unsigned                _handleAddL(uint32_t instruction);
            unsigned                _handleAddUL(uint32_t instruction);
            unsigned                _handleSubB(uint32_t instruction);
            unsigned                _handleSubUB(uint32_t instruction);
            unsigned                _handleSubH(uint32_t instruction);
            unsigned                _handleSubUH(uint32_t instruction);
            unsigned                _handleSubW(uint32_t instruction);
            unsigned                _handleSubUW(uint32_t instruction);
            unsigned                _handleSubL(uint32_t instruction);
            unsigned                _handleSubUL(uint32_t instruction);
            unsigned                _handleMulB(uint32_t instruction);
            unsigned                _handleMulUB(uint32_t instruction);
            unsigned                _handleMulH(uint32_t instruction);
            unsigned                _handleMulUH(uint32_t instruction);
            unsigned                _handleMulW(uint32_t instruction);
            unsigned                _handleMulUW(uint32_t instruction);
            unsigned                _handleMulL(uint32_t instruction);
            unsigned                _handleMulUL(uint32_t instruction);
            unsigned                _handleDivB(uint32_t instruction);
            unsigned                _handleDivUB(uint32_t instruction);
            unsigned                _handleDivH(uint32_t instruction);
            unsigned                _handleDivUH(uint32_t instruction);
            unsigned                _handleDivW(uint32_t instruction);
            unsigned                _handleDivUW(uint32_t instruction);
            unsigned                _handleDivL(uint32_t instruction);
            unsigned                _handleDivUL(uint32_t instruction);
            unsigned                _handleModB(uint32_t instruction);
            unsigned                _handleModUB(uint32_t instruction);
            unsigned                _handleModH(uint32_t instruction);
            unsigned                _handleModUH(uint32_t instruction);
            unsigned                _handleModW(uint32_t instruction);
            unsigned                _handleModUW(uint32_t instruction);
            unsigned                _handleModL(uint32_t instruction);
            unsigned                _handleModUL(uint32_t instruction);
            unsigned                _handleAbsB(uint32_t instruction);
            unsigned                _handleAbsH(uint32_t instruction);
            unsigned                _handleAbsW(uint32_t instruction);
            unsigned                _handleAbsL(uint32_t instruction);
            unsigned                _handleNegB(uint32_t instruction);
            unsigned                _handleCpl2UB(uint32_t instruction);
            unsigned                _handleNegH(uint32_t instruction);
            unsigned                _handleCpl2UH(uint32_t instruction);
            unsigned                _handleNegW(uint32_t instruction);
            unsigned                _handleCpl2UW(uint32_t instruction);
            unsigned                _handleNegL(uint32_t instruction);
            unsigned                _handleCpl2UL(uint32_t instruction);
            unsigned                _handleAddiB(uint32_t instruction);
            unsigned                _handleAddiUB(uint32_t instruction);
            unsigned                _handleAddiH(uint32_t instruction);
            unsigned                _handleAddiUH(uint32_t instruction);
            unsigned                _handleAddiW(uint32_t instruction);
            unsigned                _handleAddiUW(uint32_t instruction);
            unsigned                _handleAddiL(uint32_t instruction);
            unsigned                _handleAddiUL(uint32_t instruction);
            unsigned                _handleSubiB(uint32_t instruction);
            unsigned                _handleSubiUB(uint32_t instruction);
            unsigned                _handleSubiH(uint32_t instruction);
            unsigned                _handleSubiUH(uint32_t instruction);
            unsigned                _handleSubiW(uint32_t instruction);
            unsigned                _handleSubiUW(uint32_t instruction);
            unsigned                _handleSubiL(uint32_t instruction);
            unsigned                _handleSubiUL(uint32_t instruction);
            unsigned                _handleMuliB(uint32_t instruction);
            unsigned                _handleMuliUB(uint32_t instruction);
            unsigned                _handleMuliH(uint32_t instruction);
            unsigned                _handleMuliUH(uint32_t instruction);
            unsigned                _handleMuliW(uint32_t instruction);
            unsigned                _handleMuliUW(uint32_t instruction);
            unsigned                _handleMuliL(uint32_t instruction);
            unsigned                _handleMuliUL(uint32_t instruction);
            unsigned                _handleDiviB(uint32_t instruction);
            unsigned                _handleDiviUB(uint32_t instruction);
            unsigned                _handleDiviH(uint32_t instruction);
            unsigned                _handleDiviUH(uint32_t instruction);
            unsigned                _handleDiviW(uint32_t instruction);
            unsigned                _handleDiviUW(uint32_t instruction);
            unsigned                _handleDiviL(uint32_t instruction);
            unsigned                _handleDiviUL(uint32_t instruction);
            unsigned                _handleModiB(uint32_t instruction);
            unsigned                _handleModiUB(uint32_t instruction);
            unsigned                _handleModiH(uint32_t instruction);
            unsigned                _handleModiUH(uint32_t instruction);
            unsigned                _handleModiW(uint32_t instruction);
            unsigned                _handleModiUW(uint32_t instruction);
            unsigned                _handleModiL(uint32_t instruction);
            unsigned                _handleModiUL(uint32_t instruction);

            //  BASE (boolean)
            template <class DataType>
            void                    _executeAndSigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                result = DataType::normalizeSigned(op1 & op2);
            }

            template <class DataType>
            void                    _executeAndUnsigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                result = DataType::normalizeUnsigned(op1 & op2);
            }

            template <class DataType>
            void                    _executeOrSigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                result = DataType::normalizeSigned(op1 | op2);
            }

            template <class DataType>
            void                    _executeOrUnsigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                result = DataType::normalizeUnsigned(op1 | op2);
            }

            template <class DataType>
            void                    _executeXorSigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                result = DataType::normalizeSigned(op1 ^ op2);
            }

            template <class DataType>
            void                    _executeXorUnsigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                result = DataType::normalizeUnsigned(op1 ^ op2);
            }

            template <class DataType>
            void                    _executeImplSigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                result = DataType::normalizeSigned(~op1 | op2);
            }

            template <class DataType>
            void                    _executeImplUnsigned(uint64_t & result, uint64_t op1, uint64_t op2)
            {
                result = DataType::normalizeUnsigned(~op1 | op2);
            }

            unsigned                _handleAndB(uint32_t instruction);
            unsigned                _handleAndUB(uint32_t instruction);
            unsigned                _handleAndH(uint32_t instruction);
            unsigned                _handleAndUH(uint32_t instruction);
            unsigned                _handleAndW(uint32_t instruction);
            unsigned                _handleAndUW(uint32_t instruction);
            unsigned                _handleAndL(uint32_t instruction);
            unsigned                _handleOrB(uint32_t instruction);
            unsigned                _handleOrUB(uint32_t instruction);
            unsigned                _handleOrH(uint32_t instruction);
            unsigned                _handleOrUH(uint32_t instruction);
            unsigned                _handleOrW(uint32_t instruction);
            unsigned                _handleOrUW(uint32_t instruction);
            unsigned                _handleOrL(uint32_t instruction);
            unsigned                _handleXorB(uint32_t instruction);
            unsigned                _handleXorUB(uint32_t instruction);
            unsigned                _handleXorH(uint32_t instruction);
            unsigned                _handleXorUH(uint32_t instruction);
            unsigned                _handleXorW(uint32_t instruction);
            unsigned                _handleXorUW(uint32_t instruction);
            unsigned                _handleXorL(uint32_t instruction);
            unsigned                _handleImplB(uint32_t instruction);
            unsigned                _handleImplUB(uint32_t instruction);
            unsigned                _handleImplH(uint32_t instruction);
            unsigned                _handleImplUH(uint32_t instruction);
            unsigned                _handleImplW(uint32_t instruction);
            unsigned                _handleImplUW(uint32_t instruction);
            unsigned                _handleImplL(uint32_t instruction);
            unsigned                _handleAndiL(uint32_t instruction);
            unsigned                _handleOriL(uint32_t instruction);
            unsigned                _handleXoriL(uint32_t instruction);
            unsigned                _handleImpliL(uint32_t instruction);
            unsigned                _handleNotB(uint32_t instruction);
            unsigned                _handleNotUB(uint32_t instruction);
            unsigned                _handleNotH(uint32_t instruction);
            unsigned                _handleNotUH(uint32_t instruction);
            unsigned                _handleNotW(uint32_t instruction);
            unsigned                _handleNotUW(uint32_t instruction);
            unsigned                _handleNotL(uint32_t instruction);

            //  BASE (shifts)
            unsigned                _handleShlB(uint32_t instruction);
            unsigned                _handleShlUB(uint32_t instruction);
            unsigned                _handleShlH(uint32_t instruction);
            unsigned                _handleShlUH(uint32_t instruction);
            unsigned                _handleShlW(uint32_t instruction);
            unsigned                _handleShlUW(uint32_t instruction);
            unsigned                _handleShlL(uint32_t instruction);
            unsigned                _handleShrB(uint32_t instruction);
            unsigned                _handleShrUB(uint32_t instruction);
            unsigned                _handleShrH(uint32_t instruction);
            unsigned                _handleShrUH(uint32_t instruction);
            unsigned                _handleShrW(uint32_t instruction);
            unsigned                _handleShrUW(uint32_t instruction);
            unsigned                _handleShrL(uint32_t instruction);
            unsigned                _handleAslB(uint32_t instruction);
            unsigned                _handleAslUB(uint32_t instruction);
            unsigned                _handleAslH(uint32_t instruction);
            unsigned                _handleAslUH(uint32_t instruction);
            unsigned                _handleAslW(uint32_t instruction);
            unsigned                _handleAslUW(uint32_t instruction);
            unsigned                _handleAslL(uint32_t instruction);
            unsigned                _handleAsrB(uint32_t instruction);
            unsigned                _handleAsrUB(uint32_t instruction);
            unsigned                _handleAsrH(uint32_t instruction);
            unsigned                _handleAsrUH(uint32_t instruction);
            unsigned                _handleAsrW(uint32_t instruction);
            unsigned                _handleAsrUW(uint32_t instruction);
            unsigned                _handleAsrL(uint32_t instruction);
            unsigned                _handleRolB(uint32_t instruction);
            unsigned                _handleRolUB(uint32_t instruction);
            unsigned                _handleRolH(uint32_t instruction);
            unsigned                _handleRolUH(uint32_t instruction);
            unsigned                _handleRolW(uint32_t instruction);
            unsigned                _handleRolUW(uint32_t instruction);
            unsigned                _handleRolL(uint32_t instruction);
            unsigned                _handleRorB(uint32_t instruction);
            unsigned                _handleRorUB(uint32_t instruction);
            unsigned                _handleRorH(uint32_t instruction);
            unsigned                _handleRorUH(uint32_t instruction);
            unsigned                _handleRorW(uint32_t instruction);
            unsigned                _handleRorUW(uint32_t instruction);
            unsigned                _handleRorL(uint32_t instruction);
            unsigned                _handleShliB(uint32_t instruction);
            unsigned                _handleShliUB(uint32_t instruction);
            unsigned                _handleShliH(uint32_t instruction);
            unsigned                _handleShliUH(uint32_t instruction);
            unsigned                _handleShliW(uint32_t instruction);
            unsigned                _handleShliUW(uint32_t instruction);
            unsigned                _handleShliL(uint32_t instruction);
            unsigned                _handleShriB(uint32_t instruction);
            unsigned                _handleShriUB(uint32_t instruction);
            unsigned                _handleShriH(uint32_t instruction);
            unsigned                _handleShriUH(uint32_t instruction);
            unsigned                _handleShriW(uint32_t instruction);
            unsigned                _handleShriUW(uint32_t instruction);
            unsigned                _handleShriL(uint32_t instruction);
            unsigned                _handleAsliB(uint32_t instruction);
            unsigned                _handleAsliUB(uint32_t instruction);
            unsigned                _handleAsliH(uint32_t instruction);
            unsigned                _handleAsliUH(uint32_t instruction);
            unsigned                _handleAsliW(uint32_t instruction);
            unsigned                _handleAsliUW(uint32_t instruction);
            unsigned                _handleAsliL(uint32_t instruction);
            unsigned                _handleAsriB(uint32_t instruction);
            unsigned                _handleAsriUB(uint32_t instruction);
            unsigned                _handleAsriH(uint32_t instruction);
            unsigned                _handleAsriUH(uint32_t instruction);
            unsigned                _handleAsriW(uint32_t instruction);
            unsigned                _handleAsriUW(uint32_t instruction);
            unsigned                _handleAsriL(uint32_t instruction);
            unsigned                _handleRoliB(uint32_t instruction);
            unsigned                _handleRoliUB(uint32_t instruction);
            unsigned                _handleRoliH(uint32_t instruction);
            unsigned                _handleRoliUH(uint32_t instruction);
            unsigned                _handleRoliW(uint32_t instruction);
            unsigned                _handleRoliUW(uint32_t instruction);
            unsigned                _handleRoliL(uint32_t instruction);
            unsigned                _handleRoriB(uint32_t instruction);
            unsigned                _handleRoriUB(uint32_t instruction);
            unsigned                _handleRoriH(uint32_t instruction);
            unsigned                _handleRoriUH(uint32_t instruction);
            unsigned                _handleRoriW(uint32_t instruction);
            unsigned                _handleRoriUW(uint32_t instruction);
            unsigned                _handleRoriL(uint32_t instruction);

            //  BASE (bit manipulation)
            template <class DataType>
            void                    _executeBrevSigned(uint64_t & result, uint64_t op)
            {
                result = DataType::normalizeSigned(DataType::reverseBits(op));
            }

            template <class DataType>
            void                    _executeBrevUnsigned(uint64_t & result, uint64_t op)
            {
                result = DataType::normalizeUnsigned(DataType::reverseBits(op));
            }

            unsigned                _handleClz(uint32_t instruction);
            unsigned                _handleCtz(uint32_t instruction);
            unsigned                _handleClo(uint32_t instruction);
            unsigned                _handleCto(uint32_t instruction);
            unsigned                _handleBfeL(uint32_t instruction);
            unsigned                _handleBfeUL(uint32_t instruction);
            unsigned                _handleBfiL(uint32_t instruction);
            unsigned                _handleBrevB(uint32_t instruction);
            unsigned                _handleBrevUB(uint32_t instruction);
            unsigned                _handleBrevH(uint32_t instruction);
            unsigned                _handleBrevUH(uint32_t instruction);
            unsigned                _handleBrevW(uint32_t instruction);
            unsigned                _handleBrevUW(uint32_t instruction);
            unsigned                _handleBrevL(uint32_t instruction);

            //  BASE (conversions)
            unsigned                _handleCvtBL(uint32_t instruction);
            unsigned                _handleCvtUBL(uint32_t instruction);
            unsigned                _handleCvtHL(uint32_t instruction);
            unsigned                _handleCvtUHL(uint32_t instruction);
            unsigned                _handleCvtWL(uint32_t instruction);
            unsigned                _handleCvtUWL(uint32_t instruction);

            //  BASE (comparison)
            unsigned                _handleSeqL(uint32_t instruction);
            unsigned                _handleSneL(uint32_t instruction);
            unsigned                _handleSltL(uint32_t instruction);
            unsigned                _handleSleL(uint32_t instruction);
            unsigned                _handleSgtL(uint32_t instruction);
            unsigned                _handleSgeL(uint32_t instruction);
            unsigned                _handleSltUL(uint32_t instruction);
            unsigned                _handleSleUL(uint32_t instruction);
            unsigned                _handleSgtUL(uint32_t instruction);
            unsigned                _handleSgeUL(uint32_t instruction);
            unsigned                _handleSeqiL(uint32_t instruction);
            unsigned                _handleSneiL(uint32_t instruction);
            unsigned                _handleSltiL(uint32_t instruction);
            unsigned                _handleSleiL(uint32_t instruction);
            unsigned                _handleSgtiL(uint32_t instruction);
            unsigned                _handleSgeiL(uint32_t instruction);
            unsigned                _handleSltiUL(uint32_t instruction);
            unsigned                _handleSleiUL(uint32_t instruction);
            unsigned                _handleSgtiUL(uint32_t instruction);
            unsigned                _handleSgeiUL(uint32_t instruction);

            //  BASE (load/store)
            unsigned                _handleLB(uint32_t instruction);
            unsigned                _handleLUB(uint32_t instruction);
            unsigned                _handleLH(uint32_t instruction);
            unsigned                _handleLUH(uint32_t instruction);
            unsigned                _handleLW(uint32_t instruction);
            unsigned                _handleLUW(uint32_t instruction);
            unsigned                _handleLL(uint32_t instruction);
            unsigned                _handleXchg(uint32_t instruction);
            unsigned                _handleSB(uint32_t instruction);
            unsigned                _handleSH(uint32_t instruction);
            unsigned                _handleSW(uint32_t instruction);
            unsigned                _handleSL(uint32_t instruction);
            unsigned                _handleLir(uint32_t instruction);
            unsigned                _handleLdm(uint32_t instruction);
            unsigned                _handleStm(uint32_t instruction);

            //  BASE (flow control)
            unsigned                _handleJ(uint32_t instruction);
            unsigned                _handleJal(uint32_t instruction);
            unsigned                _handleJr(uint32_t instruction);
            unsigned                _handleJalr(uint32_t instruction);
            unsigned                _handleBeqL(uint32_t instruction);
            unsigned                _handleBneL(uint32_t instruction);
            unsigned                _handleBltL(uint32_t instruction);
            unsigned                _handleBleL(uint32_t instruction);
            unsigned                _handleBgtL(uint32_t instruction);
            unsigned                _handleBgeL(uint32_t instruction);
            unsigned                _handleBltUL(uint32_t instruction);
            unsigned                _handleBleUL(uint32_t instruction);
            unsigned                _handleBgtUL(uint32_t instruction);
            unsigned                _handleBgeUL(uint32_t instruction);
            unsigned                _handleBeqiL(uint32_t instruction);
            unsigned                _handleBneiL(uint32_t instruction);
            unsigned                _handleBltiL(uint32_t instruction);
            unsigned                _handleBleiL(uint32_t instruction);
            unsigned                _handleBgtiL(uint32_t instruction);
            unsigned                _handleBgeiL(uint32_t instruction);
            unsigned                _handleBltiUL(uint32_t instruction);
            unsigned                _handleBleiUL(uint32_t instruction);
            unsigned                _handleBgtiUL(uint32_t instruction);
            unsigned                _handleBgeiUL(uint32_t instruction);

            //  BASE (TLB control)
            unsigned                _handleIitlb(uint32_t instruction);
            unsigned                _handleIdtlb(uint32_t instruction);
            unsigned                _handleIitlbe(uint32_t instruction);
            unsigned                _handleIdtlbe(uint32_t instruction);
            unsigned                _handleIitlbc(uint32_t instruction);
            unsigned                _handleIdtlbc(uint32_t instruction);
            unsigned                _handleIitlbec(uint32_t instruction);
            unsigned                _handleIdtlbec(uint32_t instruction);

            //  BASE (cache control)
            unsigned                _handleImb(uint32_t instruction);
            unsigned                _handleDmb(uint32_t instruction);
            unsigned                _handleImbc(uint32_t instruction);
            unsigned                _handleDmbc(uint32_t instruction);

            //  BASE (I/O)
            unsigned                _handleInB(uint32_t instruction);
            unsigned                _handleInUB(uint32_t instruction);
            unsigned                _handleInH(uint32_t instruction);
            unsigned                _handleInUH(uint32_t instruction);
            unsigned                _handleInW(uint32_t instruction);
            unsigned                _handleInUW(uint32_t instruction);
            unsigned                _handleInL(uint32_t instruction);
            unsigned                _handleOutB(uint32_t instruction);
            unsigned                _handleOutH(uint32_t instruction);
            unsigned                _handleOutW(uint32_t instruction);
            unsigned                _handleOutL(uint32_t instruction);
            unsigned                _handleTstp(uint32_t instruction);
            unsigned                _handleSetp(uint32_t instruction);

            //  BASE (Misc)
            unsigned                _handleHalt(uint32_t instruction);
            unsigned                _handleIret(uint32_t instruction);
            unsigned                _handleGetfl(uint32_t instruction);
            unsigned                _handleSetfl(uint32_t instruction);
            unsigned                _handleRstfl(uint32_t instruction);
            unsigned                _handleSvc(uint32_t instruction);
            unsigned                _handleBrk(uint32_t instruction);
            unsigned                _handleCpuid(uint32_t instruction);
            unsigned                _handleSigp(uint32_t instruction);
            unsigned                _handleNop(uint32_t instruction);

            //  FLOATING POINT (data movement)
            unsigned                _handleMovD(uint32_t instruction);
            unsigned                _handleLiD(uint32_t instruction);

            //  FLOATING POINT (arithmetics)
            void                    _handleFloatingPointConditions(const hadesvm::ieee754::Environment & fpEnvironment)
            {
                //  Set flags...
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::Operand) != hadesvm::ieee754::Exception::None)
                {
                    _flags.setRealOperandFlag();
                }
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::DivisionByZero) != hadesvm::ieee754::Exception::None)
                {
                    _flags.setRealDivisionByZeroFlag();
                }
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::Overflow) != hadesvm::ieee754::Exception::None)
                {
                    _flags.setRealOverflowFlag();
                }
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::Underflow) != hadesvm::ieee754::Exception::None)
                {
                    _flags.setRealUnderflowFlag();
                }
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::Inexact) != hadesvm::ieee754::Exception::None)
                {
                    _flags.setRealInexactFlag();
                }
                //  ...and raise exceptions
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::Operand) != hadesvm::ieee754::Exception::None &&
                    _state.isRealOperandExceptionAllowed())
                {
                    _raiseProgramInterrupt(ProgramInterrupt::FOPERAND);
                }
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::DivisionByZero) != hadesvm::ieee754::Exception::None &&
                    _state.isRealDivisionByZeroExceptionAllowed())
                {
                    _raiseProgramInterrupt(ProgramInterrupt::FZDIV);
                }
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::Overflow) != hadesvm::ieee754::Exception::None &&
                    _state.isRealOverflowExceptionAllowed())
                {
                    _raiseProgramInterrupt(ProgramInterrupt::FOVERFLOW);
                }
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::Underflow) != hadesvm::ieee754::Exception::None &&
                    _state.isRealUnderflowExceptionAllowed())
                {
                    _raiseProgramInterrupt(ProgramInterrupt::FUNDERFLOW);
                }
                if ((fpEnvironment.exception | hadesvm::ieee754::Exception::Inexact) != hadesvm::ieee754::Exception::None &&
                    _state.isRealInexactExceptionAllowed())
                {
                    _raiseProgramInterrupt(ProgramInterrupt::FINEXACT);
                }
            }

            unsigned                _handleAddF(uint32_t instruction);
            unsigned                _handleAddD(uint32_t instruction);
            unsigned                _handleSubF(uint32_t instruction);
            unsigned                _handleSubD(uint32_t instruction);
            unsigned                _handleMulF(uint32_t instruction);
            unsigned                _handleMulD(uint32_t instruction);
            unsigned                _handleDivF(uint32_t instruction);
            unsigned                _handleDivD(uint32_t instruction);
            unsigned                _handleAbsF(uint32_t instruction);
            unsigned                _handleAbsD(uint32_t instruction);
            unsigned                _handleNegF(uint32_t instruction);
            unsigned                _handleNegD(uint32_t instruction);
            unsigned                _handleSqrtF(uint32_t instruction);
            unsigned                _handleSqrtD(uint32_t instruction);

            //  FLOATING POINT (conversions)
            unsigned                _handleCvtFB(uint32_t instruction);
            unsigned                _handleCvtFUB(uint32_t instruction);
            unsigned                _handleCvtFH(uint32_t instruction);
            unsigned                _handleCvtFUH(uint32_t instruction);
            unsigned                _handleCvtFW(uint32_t instruction);
            unsigned                _handleCvtFUW(uint32_t instruction);
            unsigned                _handleCvtFL(uint32_t instruction);
            unsigned                _handleCvtFUL(uint32_t instruction);
            unsigned                _handleCvtDB(uint32_t instruction);
            unsigned                _handleCvtDUB(uint32_t instruction);
            unsigned                _handleCvtDH(uint32_t instruction);
            unsigned                _handleCvtDUH(uint32_t instruction);
            unsigned                _handleCvtDW(uint32_t instruction);
            unsigned                _handleCvtDUW(uint32_t instruction);
            unsigned                _handleCvtDL(uint32_t instruction);
            unsigned                _handleCvtDUL(uint32_t instruction);
            unsigned                _handleCvtBF(uint32_t instruction);
            unsigned                _handleCvtUBF(uint32_t instruction);
            unsigned                _handleCvtHF(uint32_t instruction);
            unsigned                _handleCvtUHF(uint32_t instruction);
            unsigned                _handleCvtWF(uint32_t instruction);
            unsigned                _handleCvtUWF(uint32_t instruction);
            unsigned                _handleCvtLF(uint32_t instruction);
            unsigned                _handleCvtULF(uint32_t instruction);
            unsigned                _handleCvtBD(uint32_t instruction);
            unsigned                _handleCvtUBD(uint32_t instruction);
            unsigned                _handleCvtHD(uint32_t instruction);
            unsigned                _handleCvtUHD(uint32_t instruction);
            unsigned                _handleCvtWD(uint32_t instruction);
            unsigned                _handleCvtUWD(uint32_t instruction);
            unsigned                _handleCvtLD(uint32_t instruction);
            unsigned                _handleCvtULD(uint32_t instruction);
            unsigned                _handleCvtDF(uint32_t instruction);

            //  FLOATING POINT (comparison)
            unsigned                _handleSeqD(uint32_t instruction);
            unsigned                _handleSneD(uint32_t instruction);
            unsigned                _handleSltD(uint32_t instruction);
            unsigned                _handleSleD(uint32_t instruction);
            unsigned                _handleSgtD(uint32_t instruction);
            unsigned                _handleSgeD(uint32_t instruction);

            //  FLOATING POINT (load/store)
            unsigned                _handleLF(uint32_t instruction);
            unsigned                _handleLD(uint32_t instruction);
            unsigned                _handleSF(uint32_t instruction);
            unsigned                _handleSD(uint32_t instruction);

            //  FLOATING POINT (flow control)
            unsigned                _handleBeqD(uint32_t instruction);
            unsigned                _handleBneD(uint32_t instruction);
            unsigned                _handleBltD(uint32_t instruction);
            unsigned                _handleBleD(uint32_t instruction);
            unsigned                _handleBgtD(uint32_t instruction);
            unsigned                _handleBgeD(uint32_t instruction);
        };
    }
}

//  End of hadesvm-cereon/ProcessorCore.hpp
