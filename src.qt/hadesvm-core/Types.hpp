//
//  hadesvm-core/Types.hpp
//
//  hadesvm-core Basic VA-related types
//  TODO insert constexpr wherever possible
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
            MemorySize() : _numberOfUnits(0), _unit(Unit::B) {}
            MemorySize(uint64_t numberOfUnits, Unit unit);
            ~MemorySize() = default;

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

        //////////
        //  Represents a clock frequency
        class HADESVM_CORE_PUBLIC ClockFrequency final
        {
            //////////
            //  Types
        public:
            enum class Unit : uint64_t
            {
                Hz = UINT64_C(1),
                KHz = UINT64_C(1000),
                MHz = UINT64_C(1000) * UINT64_C(1000),
                GHz = UINT64_C(1000) * UINT64_C(1000) * UINT64_C(1000)
            };

            //////////
            //  Construction/destruction
        public:
            constexpr ClockFrequency() : _numberOfUnits(0), _unit(Unit::Hz) {}
            constexpr ClockFrequency(uint64_t numberOfUnits, Unit unit)
                :   _numberOfUnits(qMin(numberOfUnits, UINT64_MAX / static_cast<uint64_t>(unit))),
                    _unit(unit) {}
            ~ClockFrequency() = default;

            //////////
            //  Operators
        public:
            bool                operator == (const ClockFrequency & op2) const { return compare(op2) == 0; }
            bool                operator != (const ClockFrequency & op2) const { return compare(op2) != 0; }
            bool                operator <  (const ClockFrequency & op2) const { return compare(op2) <  0; }
            bool                operator <= (const ClockFrequency & op2) const { return compare(op2) <= 0; }
            bool                operator >  (const ClockFrequency & op2) const { return compare(op2) >  0; }
            bool                operator >= (const ClockFrequency & op2) const { return compare(op2) >= 0; }

            //////////
            //  Operations
        public:
            //  The number of units / unit
            uint64_t            numberOfUnits() const { return _numberOfUnits; }
            Unit                unit() const { return _unit; }

            //  Converts the ClockFrequency to the specified Unit
            uint64_t            toHz() const { return _numberOfUnits * static_cast<uint64_t>(_unit); }

            //  Builds the ClockFrequency from the specified Unit count
            static ClockFrequency   hertz(uint64_t count);
            static ClockFrequency   kilohertz(uint64_t count);
            static ClockFrequency   megahertz(uint64_t count);
            static ClockFrequency   gigahertz(uint64_t count);

            //  Compares two ClockFrequencys; returns a negative, zero or positive
            //  value if the 1st ClockFrequency is less than, equal to or greater
            //  than the 2nd ClockFrequency.
            int                 compare(const ClockFrequency & op2) const;

            //  TODO document
            QString             displayForm() const;

            //////////
            //  Implementation
        private:
            uint64_t            _numberOfUnits;
            Unit                _unit;
        };

        //////////
        //  Represents a time interval
        class HADESVM_CORE_PUBLIC TimeInterval final
        {
            //////////
            //  Types
        public:
            enum class Unit : uint64_t
            {
                Ns = UINT64_C(1),
                Us = UINT64_C(1000),
                Ms = UINT64_C(1000) * UINT64_C(1000),
                S = UINT64_C(1000) * UINT64_C(1000) * UINT64_C(1000)
            };

            //////////
            //  Construction/destruction
        public:
            TimeInterval() : _numberOfUnits(0), _unit(Unit::S) {}
            TimeInterval(uint64_t numberOfUnits, Unit unit);
            ~TimeInterval() = default;

            //////////
            //  Operators
        public:
            bool                operator == (const TimeInterval & op2) const { return compare(op2) == 0; }
            bool                operator != (const TimeInterval & op2) const { return compare(op2) != 0; }
            bool                operator <  (const TimeInterval & op2) const { return compare(op2) <  0; }
            bool                operator <= (const TimeInterval & op2) const { return compare(op2) <= 0; }
            bool                operator >  (const TimeInterval & op2) const { return compare(op2) >  0; }
            bool                operator >= (const TimeInterval & op2) const { return compare(op2) >= 0; }

            //////////
            //  Operations
        public:
            //  The number of units / unit
            uint64_t            numberOfUnits() const { return _numberOfUnits; }
            Unit                unit() const { return _unit; }

            //  Converts the TimeInterval to the specified Unit
            uint64_t            toNs() const;

            //  Builds the TimeInterval from the specified Unit count
            static TimeInterval seconds(uint64_t count);
            static TimeInterval milliseconds(uint64_t count);
            static TimeInterval microseconds(uint64_t count);
            static TimeInterval nanoseconds(uint64_t count);

            //  Compares two TimeIntervals; returns a negative, zero or positive
            //  value if the 1st TimeInterval is less than, equal to or greater
            //  than the 2nd TimeInterval.
            int                 compare(const TimeInterval & op2) const;

            //////////
            //  Implementation
        private:
            uint64_t            _numberOfUnits;
            Unit                _unit;
        };
    }

    //  Formatting and parsing
    namespace util
    {
        HADESVM_CORE_PUBLIC QString toString(const core::MemorySize & value);
        HADESVM_CORE_PUBLIC QString toString(const core::ClockFrequency & value);
        HADESVM_CORE_PUBLIC QString toString(const core::TimeInterval & value);

        template <>
        HADESVM_CORE_PUBLIC bool fromString<core::MemorySize>(const QString & s, qsizetype & scan, core::MemorySize & value);

        template <>
        HADESVM_CORE_PUBLIC bool fromString<core::ClockFrequency>(const QString & s, qsizetype & scan, core::ClockFrequency & value);

        template <>
        HADESVM_CORE_PUBLIC bool fromString<core::TimeInterval>(const QString & s, qsizetype & scan, core::TimeInterval & value);
    }
}

//  End of hadesvm-core/Types.hpp
