//
//  hadesvm-core/Types.hpp
//
//  hadesvm-core Basic VA-related types
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a size of internal memory unit
        class HADESVM_CORE_PUBLIC MemorySize final
        {
            //////////
            //  Types
        public:
            enum class Unit : uint64_t
            {
                B = UINT64_C(1),
                KB = UINT64_C(1024),
                MB = UINT64_C(1024) * UINT64_C(1024),
                GB = UINT64_C(1024) * UINT64_C(1024) * UINT64_C(1024)
            };

            //////////
            //  Construction/destruction
        public:
            MemorySize(uint64_t numberOfUnits, Unit unit);
            virtual ~MemorySize() = default;

            //////////
            //  Operators
        public:
            bool                operator == (const MemorySize & op2) const { return compare(op2) == 0; }
            bool                operator != (const MemorySize & op2) const { return compare(op2) != 0; }
            bool                operator <  (const MemorySize & op2) const { return compare(op2) <  0; }
            bool                operator <= (const MemorySize & op2) const { return compare(op2) <= 0; }
            bool                operator >  (const MemorySize & op2) const { return compare(op2) >  0; }
            bool                operator >= (const MemorySize & op2) const { return compare(op2) >= 0; }

            //////////
            //  Operations
        public:
            //  The number of units / unit
            uint64_t            numberOfUnits() const { return _numberOfUnits; }
            Unit                unit() const { return _unit; }

            //  Converts the MemorySize to the specified Unit
            uint64_t            toBytes() const;

            //  Builds the MemorySize from the specified Unit count
            static MemorySize   bytes(uint64_t count);
            static MemorySize   kilobytes(uint64_t count);
            static MemorySize   megabytes(uint64_t count);
            static MemorySize   gigabytes(uint64_t count);

            //  Compares two MemorySizes; returns a negative, zero or positive
            //  value if the 1st MemorySize is less than, equal to or greater
            //  than the 2nd MemorySize.
            int                 compare(const MemorySize & op2) const;

            //////////
            //  Implementation
        private:
            uint64_t            _numberOfUnits;
            Unit                _unit;
        };
    }
}

//  End of hadesvm-core/Types.hpp
