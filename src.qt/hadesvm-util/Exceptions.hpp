//
//  hadesvm-util/Exceptions.hpp
//
//  HadesVM exception framework
//
//////////

namespace hadesvm
{
    namespace util
    {
        //////////
        //  The common base class for all HadesVM throwables
        class HADESVM_UTIL_PUBLIC Throwable : public QException
        {
            //////////
            //  Construction/destruction/assignment
        public:
            explicit Throwable(const QString & message);
            explicit Throwable(const std::string & message);
            explicit Throwable(const char * message);

            //////////
            //  std::exception
        public:
            virtual const char *    what() const noexcept override { return _ansiMessage.c_str(); }

            //////////
            //  Operations
        public:
            QString         message() const { return _message; }

            //////////
            //  Implementation
        private:
            QString         _message;
            std::string     _ansiMessage;   //  _message as utf-8
        };

        //////////
        //  A HadesVM exception intended to be caught
        class HADESVM_UTIL_PUBLIC Exception : public Throwable
        {
            //////////
            //  Construction/destruction/assignment
        public:
            explicit Exception(const QString & message);
            explicit Exception(const std::string & message);
            explicit Exception(const char * message);

            //////////
            //  Implementation
        private:
        };

        //////////
        //  A HadesVM exception not intended to be caught (or there
        //  will be no point in catching it as repairs are impossible)
        class HADESVM_UTIL_PUBLIC Error : public Throwable
        {
            //////////
            //  Construction/destruction/assignment
        public:
            explicit Error(const QString & message);
            explicit Error(const std::string & message);
            explicit Error(const char * message);

            //////////
            //  Implementation
        private:
        };

        //////////
        //  An error in program
        class HADESVM_UTIL_PUBLIC ProgramError : public Error
        {
            //////////
            //  Construction/destruction/assignment
        public:
            explicit ProgramError(const QString & message);
            explicit ProgramError(const std::string & message);
            explicit ProgramError(const char * message);

            //////////
            //  Implementation
        private:
        };

        //////////
        //  An error in host system
        class HADESVM_UTIL_PUBLIC SystemError : public Error
        {
            //////////
            //  Construction/destruction/assignment
        public:
            explicit SystemError(const QString & message);
            explicit SystemError(const std::string & message);
            explicit SystemError(const char * message);

            //////////
            //  Implementation
        private:
        };
    }
}

//  End of hadesvm-util/Exceptions.hpp
