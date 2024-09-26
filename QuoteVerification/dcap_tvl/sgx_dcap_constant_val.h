#ifndef _SGX_DCAP_CONSTANT_VAL_H_
#define _SGX_DCAP_CONSTANT_VAL_H_

//Hardcode Intel signed QvE/QaE Identity below
//You can get such info from QvE Identity JSON file
//e.g. Get the QvE Identity JSON file from
//https://api.trustedservices.intel.com/sgx/certification/v4/qve/identity
//
#define QAE_QVE_MISC_SELECT "00000000"
#define  QAE_QVE_MISC_SELECT_MASK  "FFFFFFFF"

#define  QAE_QVE_ATTRIBUTE  "01000000000000000000000000000000"
#define  QAE_QVE_ATTRIBUTE_MASK  "FBFFFFFFFFFFFFFF0000000000000000"

//MRSIGNER of Intel signed QvE/QaE
#define QAE_QVE_MRSIGNER  "8C4F5775D796503E96137F77C68A829A0056AC8DED70140B081B094490C57BFF"

#define  QVE_PRODID 2

//Defense in depth, QvE ISV SVN in report must be greater or equal to hardcode QvE ISV SVN
#define LEAST_QVE_ISVSVN  7

//QaE prodid
#define  QAE_PRODID 3

//Defense in depth, QaE ISV SVN in report must be greater or equal to hardcode QaE ISV SVN
#define LEAST_QAE_ISVSVN  0xC

quote3_error_t enclave_identity_verify(
    int16_t prodid,
    uint16_t isv_svn,
    const sgx_report_t *p_report,
    sgx_isv_svn_t isvsvn_threshold,
    bool qae_mode
);

#endif
