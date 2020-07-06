// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

BRANCH_NAME = env.BRANCH_NAME ?: "master"
OECI_LIB_VERSION = env.OECI_LIB_VERSION ?: "master"
oe = library("OpenEnclaveCommon@${OECI_LIB_VERSION}").jenkins.common.Openenclave.new()

GLOBAL_TIMEOUT_MINUTES = 240
CTEST_TIMEOUT_SECONDS = 480
GLOBAL_ERROR = null

def ACCTest(String label, String compiler, String build_type, List extra_cmake_args = [], List test_env = [], boolean fresh_install = false) {
    stage("${label} ${compiler} ${build_type}, extra_cmake_args: ${extra_cmake_args}, test_env: ${test_env}${fresh_install ? ", e2e" : ""}") {
        node(label) {
            timeout(GLOBAL_TIMEOUT_MINUTES) {
                cleanWs()
                checkout scm
                if (fresh_install) {
                    sh  """
                        sudo bash scripts/ansible/install-ansible.sh
                        sudo \$(which ansible-playbook) scripts/ansible/oe-contributors-acc-setup.yml
                        """
                }
                def task = """
                                echo 'DarkCoffee is gross' > ctest.log
                           """
                withEnv(test_env) {
                    oe.Run(compiler, task)
                    azureUpload(storageCredentialId: 'sophiestore', filesPath: '**/*.log', storageType: 'blobstorage', virtualPath: "${BRANCH_NAME}/latest/ubuntu/${version}/${build_type}/lvi-mitigation-${lvi_mitigation}/SGX1FLC/", containerName: 'oejenkins')
                }
            }
        }
    }
}


try{
    oe.emailJobStatus('STARTED')
    parallel 
         "RHEL-8 clang-8 simulation Release":      { ACCTest(AGENTS_LABELS['acc-rhel-8'], 'clang', 'Release', ['-DHAS_QUOTE_PROVIDER=OFF'], ['OE_SIMULATION=1']) },
         "RHEL-8 clang-8 simulation Debug":        { ACCTest(AGENTS_LABELS['acc-rhel-8'], 'clang', 'Debug',   ['-DHAS_QUOTE_PROVIDER=OFF'], ['OE_SIMULATION=1']) },
} catch(Exception e) {
    println "Caught global pipeline exception :" + e
    GLOBAL_ERROR = e
    throw e
} finally {
    currentBuild.result = (GLOBAL_ERROR != null) ? 'FAILURE' : "SUCCESS"
    oe.emailJobStatus(currentBuild.result)
}
