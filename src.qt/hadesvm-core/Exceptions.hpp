//
//  hadesvm-core/Exceptions.hpp
//
//  hadesvm-core VM exceptions
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  The common base class for all VM exceptions
        class HADESVM_CORE_PUBLIC VirtualApplianceException : public hadesvm::util::Exception
        {
            //////////
            //  Construction/destruction/assignment
        public:
            explicit VirtualApplianceException(const QString & message);
            explicit VirtualApplianceException(const std::string & message);
            explicit VirtualApplianceException(const char * message);
        };
    }
}

//  End of hadesvm-core/Exceptions.hpp
